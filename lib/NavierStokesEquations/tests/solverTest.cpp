#include "lib/NavierStokesEquations/solver.h"

#include <cmath>
#include <vector>

#include "lib/NavierStokesEquations/diagnostics.h"
#include "lib/NavierStokesEquations/obstacle.h"

#include "gtest/gtest.h"

using namespace navierStokes;

namespace {

// The Taylor-Green vortex is an exact solution of the two-dimensional
// Navier-Stokes equations on a doubly periodic domain of side 2 pi: the
// nonlinear term is balanced by the pressure gradient, leaving a pure decay,
//   u = -cos(x) sin(y) e^{-2 nu t},   v = sin(x) cos(y) e^{-2 nu t}.
double taylorGreenU(double x, double y, double nu, double t) {
    return -std::cos(x) * std::sin(y) * std::exp(-2.0 * nu * t);
}

double taylorGreenV(double x, double y, double nu, double t) {
    return std::sin(x) * std::cos(y) * std::exp(-2.0 * nu * t);
}

void setTaylorGreen(NavierStokesSolver* solver, double nu) {
    solver->setVelocity([nu](double x, double y) { return taylorGreenU(x, y, nu, 0.0); },
                        [nu](double x, double y) { return taylorGreenV(x, y, nu, 0.0); });
}

// Root-mean-square error of u against the exact Taylor-Green solution.
double taylorGreenErrorU(const NavierStokesSolver& solver, double nu) {
    const StaggeredGrid& grid = solver.grid();
    double sumSquares = 0.0;
    int count = 0;
    for (int i = 0; i <= grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const double exact = taylorGreenU(grid.xU(i), grid.yU(j), nu, solver.time());
            const double error = solver.u()(i, j) - exact;
            sumSquares += error * error;
            ++count;
        }
    }
    return std::sqrt(sumSquares / count);
}

}  // namespace

TEST(Solver, FluidAtRestStaysAtRest) {
    const StaggeredGrid grid(8, 8, 1.0, 1.0);
    SolverOptions options;
    options.viscosity = 0.05;
    NavierStokesSolver solver(grid, BoundaryConditions::cavity(0.0), options);
    for (int step = 0; step < 20; ++step) {
        solver.step(0.01);
    }
    EXPECT_DOUBLE_EQ(solver.u().maxAbs(), 0.0);
    EXPECT_DOUBLE_EQ(solver.v().maxAbs(), 0.0);
    EXPECT_DOUBLE_EQ(solver.kineticEnergy(), 0.0);
    EXPECT_DOUBLE_EQ(solver.time(), 20 * 0.01);
}

// The divergence and gradient operators of a staggered grid are exact
// transposes, so the projection removes the divergence to round-off. This is
// the single most important invariant of the scheme, and it must hold for every
// combination of boundary conditions.
TEST(Solver, ProjectionLeavesNoDivergence) {
    const StaggeredGrid grid(16, 12, 2.0, 1.0);
    SolverOptions options;
    options.viscosity = 0.02;
    options.poisson.tolerance = 1e-12;
    options.poisson.maxIterations = 5000;

    struct Case {
        const char* name;
        BoundaryConditions bc;
        bool obstacle;
    };
    const Case cases[] = {
        {"cavity", BoundaryConditions::cavity(1.0), false},
        {"doublyPeriodic", BoundaryConditions::doublyPeriodic(), false},
        {"periodicChannel", BoundaryConditions::periodicChannel(), false},
        {"inflowOutflow", BoundaryConditions::inflowOutflow(1.0), false},
        {"freeStream", BoundaryConditions::freeStream(1.0), false},
        {"inflowOutflow+disk", BoundaryConditions::inflowOutflow(1.0), true},
        {"periodicChannel+disk", BoundaryConditions::periodicChannel(), true},
    };

    for (const Case& testCase : cases) {
        NavierStokesSolver solver(grid, testCase.bc, options);
        if (testCase.obstacle) {
            solver.setSolid(solidDisk(grid, 0.5, 0.5, 0.15));
        }
        solver.setUniformVelocity(1.0, 0.0);
        solver.projectVelocity();
        EXPECT_LT(solver.maxDivergence(), 1e-9) << testCase.name << " after projectVelocity";
        for (int step = 0; step < 10; ++step) {
            const StepReport report = solver.stepAdaptive(0.01);
            EXPECT_LT(report.maxDivergence, 1e-9) << testCase.name << " at step " << step;
            // The reported divergence describes the state the caller can see,
            // so re-measuring it must give the same answer.
            EXPECT_DOUBLE_EQ(solver.maxDivergence(), report.maxDivergence) << testCase.name;
        }
    }
}

TEST(Solver, ProjectVelocityRemovesDivergenceOfAnArbitraryField) {
    const StaggeredGrid grid(16, 16, 1.0, 1.0);
    SolverOptions options;
    options.poisson.tolerance = 1e-12;
    options.poisson.maxIterations = 5000;
    NavierStokesSolver solver(grid, BoundaryConditions::cavity(0.0), options);

    // A strongly divergent field: a pure radial expansion.
    solver.setVelocity([](double x, double) { return x - 0.5; }, [](double, double y) { return y - 0.5; });
    EXPECT_GT(solver.maxDivergence(), 1.0);
    solver.projectVelocity();
    EXPECT_LT(solver.maxDivergence(), 1e-10);
}

TEST(Solver, TaylorGreenVortexConvergesAtSecondOrder) {
    const double nu = 0.05;
    const double tEnd = 0.5;
    double previousError = 0.0;
    for (int n : {16, 32}) {
        const StaggeredGrid grid(n, n, 2.0 * M_PI, 2.0 * M_PI);
        SolverOptions options;
        options.viscosity = nu;
        options.poisson.tolerance = 1e-12;
        options.poisson.maxIterations = 5000;
        NavierStokesSolver solver(grid, BoundaryConditions::doublyPeriodic(), options);
        setTaylorGreen(&solver, nu);

        // dt proportional to dx^2 keeps the first-order-in-time splitting error
        // at the same order as the second-order spatial error.
        const double dt = 0.05 * grid.dx * grid.dx;
        const int steps = static_cast<int>(std::ceil(tEnd / dt));
        for (int step = 0; step < steps; ++step) {
            solver.step(tEnd / steps);
        }

        const double error = taylorGreenErrorU(solver, nu);
        if (previousError > 0.0) {
            const double order = std::log2(previousError / error);
            EXPECT_GT(order, 1.8) << "observed order at n = " << n;
            EXPECT_LT(order, 2.2) << "observed order at n = " << n;
        } else {
            EXPECT_LT(error, 1e-3);
        }
        previousError = error;
    }
}

// The kinetic energy of the Taylor-Green vortex decays as e^{-4 nu t}, since
// the velocity itself decays as e^{-2 nu t}.
TEST(Solver, TaylorGreenEnergyDecaysAtTheAnalyticRate) {
    const double nu = 0.05;
    const StaggeredGrid grid(32, 32, 2.0 * M_PI, 2.0 * M_PI);
    SolverOptions options;
    options.viscosity = nu;
    options.poisson.tolerance = 1e-12;
    NavierStokesSolver solver(grid, BoundaryConditions::doublyPeriodic(), options);
    setTaylorGreen(&solver, nu);

    const double initialEnergy = solver.kineticEnergy();
    EXPECT_GT(initialEnergy, 0.0);
    const double dt = 0.05 * grid.dx * grid.dx;
    for (int step = 0; step < 200; ++step) {
        solver.step(dt);
    }
    const double expected = initialEnergy * std::exp(-4.0 * nu * solver.time());
    EXPECT_NEAR(solver.kineticEnergy() / expected, 1.0, 0.01);
}

// Plane Poiseuille flow: a periodic channel driven by a constant body force G
// has the steady parabolic profile u(y) = G y (h - y) / (2 nu).
TEST(Solver, PoiseuilleFlowMatchesTheAnalyticProfile) {
    const double height = 1.0;
    const double nu = 0.1;
    const double force = 0.1;
    const double peak = force * height * height / (8.0 * nu);

    double previousError = 0.0;
    for (int ny : {16, 32}) {
        const StaggeredGrid grid(4, ny, 1.0, height);
        SolverOptions options;
        options.viscosity = nu;
        options.forceX = force;
        options.poisson.tolerance = 1e-12;
        NavierStokesSolver solver(grid, BoundaryConditions::periodicChannel(), options);
        while (solver.time() < 25.0) {
            solver.stepAdaptive(0.01);
        }

        double error = 0.0;
        for (int j = 0; j < ny; ++j) {
            const double y = grid.yU(j);
            const double exact = force / (2.0 * nu) * y * (height - y);
            error = std::max(error, std::fabs(solver.u()(2, j) - exact));
        }
        EXPECT_LT(error / peak, 5e-3) << "relative error at ny = " << ny;
        if (previousError > 0.0) {
            const double order = std::log2(previousError / error);
            EXPECT_GT(order, 1.8);
            EXPECT_LT(order, 2.2);
        }
        previousError = error;

        // The flow is unidirectional and steady.
        EXPECT_LT(solver.v().maxAbs(), 1e-9);
        EXPECT_LT(solver.maxDivergence(), 1e-10);
    }
}

// Lid-driven cavity at Re = 100. The reference minimum of the u profile on the
// vertical centreline is -0.2109 at y = 0.4531 (Ghia, Ghia & Shin, 1982); a
// 32x32 grid should land close to it.
TEST(Solver, LidDrivenCavityReproducesTheReferenceCentreline) {
    const int n = 32;
    const StaggeredGrid grid(n, n, 1.0, 1.0);
    SolverOptions options;
    options.viscosity = 0.01;  // Re = U L / nu = 100
    options.poisson.tolerance = 1e-10;
    options.poisson.maxIterations = 5000;
    NavierStokesSolver solver(grid, BoundaryConditions::cavity(1.0), options);
    while (solver.time() < 20.0) {
        solver.stepAdaptive(0.05);
    }

    double minimumU = 1.0;
    double minimumY = 0.0;
    for (int j = 0; j < n; ++j) {
        const double u = solver.uAtCenter(n / 2, j);
        if (u < minimumU) {
            minimumU = u;
            minimumY = grid.yP(j);
        }
    }
    EXPECT_NEAR(minimumU, -0.211, 0.03);
    EXPECT_NEAR(minimumY, 0.453, 0.06);

    // The lid drives a single clockwise primary vortex, so the flow near the
    // lid runs with the lid and the return flow below it runs against it.
    EXPECT_GT(solver.uAtCenter(n / 2, n - 1), 0.3);
    EXPECT_LT(solver.maxDivergence(), 1e-9);
}

TEST(Solver, ObstacleFacesCarryNoFlow) {
    const StaggeredGrid grid(24, 12, 2.0, 1.0);
    SolverOptions options;
    options.viscosity = 0.02;
    options.poisson.tolerance = 1e-10;
    options.poisson.maxIterations = 5000;
    NavierStokesSolver solver(grid, BoundaryConditions::freeStream(1.0), options);
    const std::vector<char> solid = solidDisk(grid, 0.5, 0.5, 0.2);
    ASSERT_GT(solidCellCount(solid), 0);
    solver.setSolid(solid);
    solver.setUniformVelocity(1.0, 0.0);
    solver.projectVelocity();

    for (int step = 0; step < 25; ++step) {
        solver.stepAdaptive(0.01);
    }

    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            if (!solver.solidCell(i, j)) {
                continue;
            }
            // Every face of a solid cell is held at zero, so no fluid crosses
            // the obstacle and the velocity inside it vanishes.
            EXPECT_DOUBLE_EQ(solver.u()(i, j), 0.0);
            EXPECT_DOUBLE_EQ(solver.u()(i + 1, j), 0.0);
            EXPECT_DOUBLE_EQ(solver.v()(i, j), 0.0);
            EXPECT_DOUBLE_EQ(solver.v()(i, j + 1), 0.0);
        }
    }
    EXPECT_LT(solver.maxDivergence(), 1e-8);
    // The obstacle blocks the stream, so the flow has to speed up beside it.
    EXPECT_GT(solver.maxVelocity(), 1.0);
}

TEST(Solver, UpwindAdvectionStaysBoundedAtHighReynoldsNumber) {
    const StaggeredGrid grid(32, 32, 1.0, 1.0);
    SolverOptions options;
    options.viscosity = 1e-4;  // Re = 10000 on a coarse grid
    options.advection = AdvectionScheme::Upwind;
    options.poisson.tolerance = 1e-10;
    NavierStokesSolver solver(grid, BoundaryConditions::cavity(1.0), options);
    for (int step = 0; step < 200; ++step) {
        const StepReport report = solver.stepAdaptive(0.01);
        ASSERT_TRUE(std::isfinite(report.maxVelocity)) << "at step " << step;
    }
    // The lid can only drive fluid up to its own speed; a diffusive first-order
    // scheme has no mechanism to exceed it by much.
    EXPECT_LT(solver.maxVelocity(), 1.5);
    EXPECT_LT(solver.maxDivergence(), 1e-7);
}

TEST(Solver, TimeStepLimitFollowsTheStabilityConditions) {
    const StaggeredGrid grid(10, 20, 1.0, 1.0);  // dx = 0.1, dy = 0.05
    const double inverseSquares = 1.0 / (grid.dx * grid.dx) + 1.0 / (grid.dy * grid.dy);

    // The limit is the smallest of the individual conditions, so each one has
    // to be isolated by making the others inactive.
    SolverOptions viscous;
    viscous.viscosity = 0.1;
    viscous.cflSafety = 0.5;
    NavierStokesSolver viscousFluid(grid, BoundaryConditions::cavity(0.0), viscous);
    EXPECT_NEAR(viscousFluid.maxStableTimeStep(), 0.5 * 0.5 / (viscous.viscosity * inverseSquares), 1e-15);
    // Halving the safety factor halves the step.
    viscous.cflSafety = 0.25;
    NavierStokesSolver halfSafety(grid, BoundaryConditions::cavity(0.0), viscous);
    EXPECT_NEAR(halfSafety.maxStableTimeStep(), 0.5 * viscousFluid.maxStableTimeStep(), 1e-15);

    // With a barely viscous fluid the convective condition binds, on whichever
    // axis the fluid crosses a cell fastest.
    SolverOptions convective;
    convective.viscosity = 1e-4;
    convective.cflSafety = 0.5;
    NavierStokesSolver movingFluid(grid, BoundaryConditions::cavity(0.0), convective);
    movingFluid.setUniformVelocity(0.0, 1.0);
    EXPECT_NEAR(movingFluid.maxStableTimeStep(), 0.5 * grid.dy, 1e-15);
    movingFluid.setUniformVelocity(1.0, 0.0);
    EXPECT_NEAR(movingFluid.maxStableTimeStep(), 0.5 * grid.dx, 1e-15);

    // A body force on a fluid at rest limits the step on its own.
    SolverOptions forced;
    forced.viscosity = 0.0;
    forced.forceY = 8.0;
    forced.cflSafety = 0.5;
    NavierStokesSolver forcedFluid(grid, BoundaryConditions::cavity(0.0), forced);
    EXPECT_NEAR(forcedFluid.maxStableTimeStep(), 0.5 * std::sqrt(2.0 * grid.minSpacing() / forced.forceY), 1e-15);

    // Nothing limits an inviscid fluid at rest, so the caller has to.
    SolverOptions inviscid;
    inviscid.viscosity = 0.0;
    NavierStokesSolver stillFluid(grid, BoundaryConditions::cavity(0.0), inviscid);
    EXPECT_FALSE(std::isfinite(stillFluid.maxStableTimeStep()));
    EXPECT_DOUBLE_EQ(stillFluid.stepAdaptive(0.25).dt, 0.25);
}

// setVelocity samples u at the vertical faces and v at the horizontal ones,
// then applies the boundary conditions. The sampled functions are periodic so
// that the boundary pass agrees with them and the check can be an equality.
TEST(Solver, SetVelocitySamplesAtFacePositions) {
    const StaggeredGrid grid(8, 8, 2.0, 1.0);
    const auto uExact = [&grid](double x, double y) {
        return std::sin(2.0 * M_PI * x / grid.lengthX) + 2.0 * std::sin(2.0 * M_PI * y / grid.lengthY);
    };
    const auto vExact = [&grid](double x, double y) {
        return 3.0 * std::sin(2.0 * M_PI * x / grid.lengthX) - std::sin(2.0 * M_PI * y / grid.lengthY);
    };

    NavierStokesSolver solver(grid, BoundaryConditions::doublyPeriodic(), SolverOptions());
    solver.setVelocity(uExact, vExact);
    for (int i = 0; i <= grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_NEAR(solver.u()(i, j), uExact(grid.xU(i), grid.yU(j)), 1e-12) << "u at " << i << ", " << j;
        }
    }
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j <= grid.ny; ++j) {
            EXPECT_NEAR(solver.v()(i, j), vExact(grid.xV(i), grid.yV(j)), 1e-12) << "v at " << i << ", " << j;
        }
    }
    // The ghost cells wrap around to the far side of the domain.
    for (int j = 0; j < grid.ny; ++j) {
        EXPECT_DOUBLE_EQ(solver.u()(-1, j), solver.u()(grid.nx - 1, j));
    }
    for (int i = 0; i < grid.nx; ++i) {
        EXPECT_DOUBLE_EQ(solver.v()(i, -1), solver.v()(i, grid.ny - 1));
    }

    // A no-slip wall overrides the sampled values on the faces it owns, which
    // is why setVelocity has to be followed by projectVelocity.
    NavierStokesSolver cavity(grid, BoundaryConditions::cavity(1.0), SolverOptions());
    cavity.setUniformVelocity(0.7, 0.3);
    for (int j = 0; j < grid.ny; ++j) {
        EXPECT_DOUBLE_EQ(cavity.u()(0, j), 0.0);
        EXPECT_DOUBLE_EQ(cavity.u()(grid.nx, j), 0.0);
        EXPECT_DOUBLE_EQ(cavity.u()(1, j), 0.7);
    }
    for (int i = 0; i < grid.nx; ++i) {
        EXPECT_DOUBLE_EQ(cavity.v()(i, 0), 0.0);
        EXPECT_DOUBLE_EQ(cavity.v()(i, grid.ny), 0.0);
    }
    // The lid moves at 1, so the ghost row above it mirrors to 2 - 0.7. The
    // two end columns are left out: their u faces sit on the side walls and
    // are held at zero, which mirrors to 2 instead.
    for (int i = 1; i < grid.nx; ++i) {
        EXPECT_DOUBLE_EQ(cavity.u()(i, grid.ny), 2.0 * 1.0 - 0.7);
    }
}

TEST(Solver, StepReportDescribesTheStep) {
    const StaggeredGrid grid(16, 16, 2.0 * M_PI, 2.0 * M_PI);
    SolverOptions options;
    options.viscosity = 0.05;
    NavierStokesSolver solver(grid, BoundaryConditions::doublyPeriodic(), options);
    setTaylorGreen(&solver, 0.05);

    const StepReport report = solver.step(0.001);
    EXPECT_DOUBLE_EQ(report.dt, 0.001);
    EXPECT_DOUBLE_EQ(report.time, 0.001);
    EXPECT_NEAR(report.maxVelocity, 1.0, 0.1);
    EXPECT_TRUE(report.poisson.converged);
    EXPECT_GT(report.poisson.iterations, 0);
}
