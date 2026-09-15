#include "lib/NavierStokesEquations/poisson.h"

#include <cmath>
#include <vector>

#include "gtest/gtest.h"

using namespace navierStokes;

namespace {

// A deterministic pseudo-random field, used where the point is to exercise the
// operator on something with no structure for a bug to hide behind.
Field2D noiseField(int nx, int ny, unsigned seed) {
    Field2D f(nx, ny, 0);
    unsigned state = seed;
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            state = state * 1664525u + 1013904223u;
            f(i, j) = static_cast<double>(state >> 8) / static_cast<double>(1u << 24) - 0.5;
        }
    }
    return f;
}

double dotOverCells(const PressurePoisson& poisson, const Field2D& a, const Field2D& b) {
    double total = 0.0;
    for (int i = 0; i < a.nx(); ++i) {
        for (int j = 0; j < a.ny(); ++j) {
            if (poisson.isFluid(i, j)) {
                total += a(i, j) * b(i, j);
            }
        }
    }
    return total;
}

std::vector<char> diskMask(const StaggeredGrid& grid, double radius) {
    std::vector<char> solid(static_cast<size_t>(grid.nx) * static_cast<size_t>(grid.ny), 0);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const double dx = grid.xP(i) - 0.5 * grid.lengthX;
            const double dy = grid.yP(j) - 0.5 * grid.lengthY;
            if (std::hypot(dx, dy) < radius) {
                solid[static_cast<size_t>(i) * static_cast<size_t>(grid.ny) + static_cast<size_t>(j)] = 1;
            }
        }
    }
    return solid;
}

}  // namespace

// The conjugate gradient method is only valid for a symmetric operator, and
// dropping the flux terms at walls and at solid cells is what keeps the
// discrete Laplacian symmetric. Check <Lx, y> == <x, Ly> directly.
TEST(Poisson, OperatorIsSymmetric) {
    const StaggeredGrid grid(11, 9, 1.3, 0.7);
    const BoundaryConditions configurations[] = {
        BoundaryConditions::cavity(1.0),
        BoundaryConditions::doublyPeriodic(),
        BoundaryConditions::periodicChannel(),
        BoundaryConditions::inflowOutflow(1.0),
    };
    for (const BoundaryConditions& bc : configurations) {
        for (bool withObstacle : {false, true}) {
            PressurePoisson poisson(grid, bc);
            if (withObstacle) {
                poisson.setSolid(diskMask(grid, 0.2));
            }
            const Field2D x = noiseField(grid.nx, grid.ny, 12345u);
            const Field2D y = noiseField(grid.nx, grid.ny, 777u);
            Field2D lx(grid.nx, grid.ny, 0);
            Field2D ly(grid.nx, grid.ny, 0);
            poisson.applyLaplacian(x, lx);
            poisson.applyLaplacian(y, ly);
            EXPECT_NEAR(dotOverCells(poisson, lx, y), dotOverCells(poisson, x, ly), 1e-9);
        }
    }
}

// Constants are in the null space of an all-Neumann (or periodic) Laplacian.
TEST(Poisson, ConstantsAreAnnihilated) {
    const StaggeredGrid grid(8, 6, 1.0, 1.0);
    for (const BoundaryConditions& bc : {BoundaryConditions::cavity(0.0), BoundaryConditions::doublyPeriodic()}) {
        PressurePoisson poisson(grid, bc);
        Field2D p(grid.nx, grid.ny, 0);
        p.fill(3.25);
        Field2D out(grid.nx, grid.ny, 0);
        poisson.applyLaplacian(p, out);
        EXPECT_NEAR(out.maxAbs(), 0.0, 1e-12);
    }
}

// Round trip through the discrete operator: manufacture a right-hand side from
// a known zero-mean field, then check the solver recovers that field. This
// isolates the solver from any discretisation error.
TEST(Poisson, RecoversAManufacturedSolution) {
    const StaggeredGrid grid(24, 20, 1.0, 1.5);
    const BoundaryConditions configurations[] = {
        BoundaryConditions::cavity(0.0),
        BoundaryConditions::doublyPeriodic(),
        BoundaryConditions::periodicChannel(),
    };
    for (const BoundaryConditions& bc : configurations) {
        for (PoissonMethod method : {PoissonMethod::ConjugateGradient, PoissonMethod::Sor}) {
            PressurePoisson poisson(grid, bc);
            Field2D expected = noiseField(grid.nx, grid.ny, 4242u);
            poisson.subtractFluidMean(expected);

            Field2D rhs(grid.nx, grid.ny, 0);
            poisson.applyLaplacian(expected, rhs);

            PoissonOptions options;
            options.method = method;
            options.tolerance = 1e-12;
            options.maxIterations = 20000;
            Field2D p(grid.nx, grid.ny, 0);
            const PoissonReport report = poisson.solve(p, rhs, options);
            EXPECT_TRUE(report.converged) << "residual " << report.residual;

            double maxError = 0.0;
            for (int i = 0; i < grid.nx; ++i) {
                for (int j = 0; j < grid.ny; ++j) {
                    maxError = std::max(maxError, std::fabs(p(i, j) - expected(i, j)));
                }
            }
            EXPECT_LT(maxError, 1e-8);
        }
    }
}

TEST(Poisson, SolutionHasZeroMean) {
    const StaggeredGrid grid(10, 10, 1.0, 1.0);
    PressurePoisson poisson(grid, BoundaryConditions::cavity(0.0));
    Field2D expected = noiseField(grid.nx, grid.ny, 99u);
    Field2D rhs(grid.nx, grid.ny, 0);
    poisson.applyLaplacian(expected, rhs);

    Field2D p(grid.nx, grid.ny, 0);
    p.fill(1000.0);  // an initial guess far from zero mean
    poisson.solve(p, rhs, PoissonOptions());
    EXPECT_NEAR(poisson.fluidMean(p), 0.0, 1e-9);
}

// An inconsistent right-hand side (non-zero mean) has no solution at all. The
// solver projects the mean out rather than diverging, and must then satisfy the
// projected problem exactly.
TEST(Poisson, InconsistentRightHandSideIsProjected) {
    const StaggeredGrid grid(12, 12, 1.0, 1.0);
    PressurePoisson poisson(grid, BoundaryConditions::cavity(0.0));
    Field2D rhs(grid.nx, grid.ny, 0);
    rhs.fill(1.0);  // pure null-space component: mean 1, no solvable part

    PoissonOptions options;
    options.tolerance = 1e-12;
    Field2D p(grid.nx, grid.ny, 0);
    const PoissonReport report = poisson.solve(p, rhs, options);
    EXPECT_TRUE(report.converged);
    EXPECT_NEAR(p.maxAbs(), 0.0, 1e-9);
}

TEST(Poisson, SolvesWithAnObstacle) {
    const StaggeredGrid grid(20, 20, 1.0, 1.0);
    PressurePoisson poisson(grid, BoundaryConditions::cavity(0.0));
    const std::vector<char> solid = diskMask(grid, 0.15);
    poisson.setSolid(solid);
    EXPECT_LT(poisson.fluidCellCount(), grid.nx * grid.ny);
    EXPECT_GT(poisson.fluidCellCount(), 0);

    Field2D expected = noiseField(grid.nx, grid.ny, 31337u);
    poisson.subtractFluidMean(expected);  // also zeroes the solid cells
    Field2D rhs(grid.nx, grid.ny, 0);
    poisson.applyLaplacian(expected, rhs);

    PoissonOptions options;
    options.tolerance = 1e-12;
    options.maxIterations = 20000;
    Field2D p(grid.nx, grid.ny, 0);
    const PoissonReport report = poisson.solve(p, rhs, options);
    EXPECT_TRUE(report.converged) << "residual " << report.residual;
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_NEAR(p(i, j), expected(i, j), 1e-8);
            if (!poisson.isFluid(i, j)) {
                EXPECT_DOUBLE_EQ(p(i, j), 0.0);
            }
        }
    }
}

// Second-order accuracy of the Neumann discretisation against an analytic
// solution: p = cos(pi x) cos(pi y) has zero normal derivative on every wall.
TEST(Poisson, NeumannDiscretisationIsSecondOrder) {
    double previousError = 0.0;
    for (int n : {16, 32, 64}) {
        const StaggeredGrid grid(n, n, 1.0, 1.0);
        PressurePoisson poisson(grid, BoundaryConditions::cavity(0.0));

        Field2D rhs(n, n, 0);
        Field2D exact(n, n, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                const double value = std::cos(M_PI * grid.xP(i)) * std::cos(M_PI * grid.yP(j));
                exact(i, j) = value;
                rhs(i, j) = -2.0 * M_PI * M_PI * value;
            }
        }
        poisson.subtractFluidMean(exact);

        PoissonOptions options;
        options.tolerance = 1e-12;
        options.maxIterations = 20000;
        Field2D p(n, n, 0);
        ASSERT_TRUE(poisson.solve(p, rhs, options).converged);

        double error = 0.0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                error = std::max(error, std::fabs(p(i, j) - exact(i, j)));
            }
        }
        if (previousError > 0.0) {
            const double order = std::log2(previousError / error);
            EXPECT_GT(order, 1.8) << "at n = " << n;
            EXPECT_LT(order, 2.2) << "at n = " << n;
        }
        previousError = error;
    }
}

TEST(Poisson, SorAndConjugateGradientAgree) {
    const StaggeredGrid grid(16, 16, 1.0, 1.0);
    PressurePoisson poisson(grid, BoundaryConditions::periodicChannel());
    Field2D source = noiseField(grid.nx, grid.ny, 20260915u);
    poisson.subtractFluidMean(source);
    Field2D rhs(grid.nx, grid.ny, 0);
    poisson.applyLaplacian(source, rhs);

    PoissonOptions cgOptions;
    cgOptions.tolerance = 1e-11;
    cgOptions.maxIterations = 20000;
    PoissonOptions sorOptions = cgOptions;
    sorOptions.method = PoissonMethod::Sor;

    Field2D cg(grid.nx, grid.ny, 0);
    Field2D sor(grid.nx, grid.ny, 0);
    ASSERT_TRUE(poisson.solve(cg, rhs, cgOptions).converged);
    ASSERT_TRUE(poisson.solve(sor, rhs, sorOptions).converged);

    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_NEAR(cg(i, j), sor(i, j), 1e-7);
        }
    }
}

TEST(Poisson, ReportsFailureWhenIterationsRunOut) {
    const StaggeredGrid grid(32, 32, 1.0, 1.0);
    PressurePoisson poisson(grid, BoundaryConditions::cavity(0.0));
    Field2D source = noiseField(grid.nx, grid.ny, 5u);
    Field2D rhs(grid.nx, grid.ny, 0);
    poisson.applyLaplacian(source, rhs);

    PoissonOptions options;
    options.maxIterations = 1;
    options.tolerance = 1e-14;
    Field2D p(grid.nx, grid.ny, 0);
    const PoissonReport report = poisson.solve(p, rhs, options);
    EXPECT_FALSE(report.converged);
    EXPECT_EQ(report.iterations, 1);
    EXPECT_GT(report.residual, 0.0);
}
