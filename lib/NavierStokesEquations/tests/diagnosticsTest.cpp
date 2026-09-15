#include "lib/NavierStokesEquations/diagnostics.h"

#include <cmath>
#include <functional>

#include "lib/NavierStokesEquations/boundary.h"
#include "lib/NavierStokesEquations/solver.h"

#include "gtest/gtest.h"

using namespace navierStokes;

namespace {

// The diagnostics read one cell past the domain, so the samplers fill the halo
// too rather than relying on a boundary pass.
Field2D sampleU(const StaggeredGrid& grid, const std::function<double(double, double)>& f) {
    Field2D u(grid.nx + 1, grid.ny, 1);
    for (int i = -1; i <= grid.nx + 1; ++i) {
        for (int j = -1; j <= grid.ny; ++j) {
            u(i, j) = f(grid.xU(i), grid.yU(j));
        }
    }
    return u;
}

Field2D sampleV(const StaggeredGrid& grid, const std::function<double(double, double)>& f) {
    Field2D v(grid.nx, grid.ny + 1, 1);
    for (int i = -1; i <= grid.nx; ++i) {
        for (int j = -1; j <= grid.ny + 1; ++j) {
            v(i, j) = f(grid.xV(i), grid.yV(j));
        }
    }
    return v;
}

}  // namespace

// Every operator below is exact on a linear field, which is what makes these
// assertions equalities rather than tolerances.
TEST(Diagnostics, DivergenceOfALinearFieldIsExact) {
    const StaggeredGrid grid(6, 4, 3.0, 1.0);
    const Field2D u = sampleU(grid, [](double x, double) { return 2.0 * x; });
    const Field2D v = sampleV(grid, [](double, double y) { return -3.0 * y; });
    const Field2D divergence = divergenceField(grid, u, v);
    ASSERT_EQ(divergence.nx(), grid.nx);
    ASSERT_EQ(divergence.ny(), grid.ny);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_NEAR(divergence(i, j), 2.0 - 3.0, 1e-12) << "at " << i << ", " << j;
        }
    }
}

TEST(Diagnostics, UniformFlowHasNoDivergenceAndNoVorticity) {
    const StaggeredGrid grid(5, 5, 1.0, 1.0);
    const Field2D u = sampleU(grid, [](double, double) { return 1.5; });
    const Field2D v = sampleV(grid, [](double, double) { return -0.5; });
    EXPECT_DOUBLE_EQ(divergenceField(grid, u, v).maxAbs(), 0.0);
    EXPECT_DOUBLE_EQ(vorticityField(grid, u, v).maxAbs(), 0.0);
    EXPECT_DOUBLE_EQ(vorticityAtCorners(grid, u, v).maxAbs(), 0.0);
}

// Solid-body rotation u = (-y, x) has vorticity 2 everywhere.
TEST(Diagnostics, VorticityOfSolidBodyRotation) {
    const StaggeredGrid grid(8, 6, 2.0, 1.5);
    const Field2D u = sampleU(grid, [](double, double y) { return -y; });
    const Field2D v = sampleV(grid, [](double x, double) { return x; });

    const Field2D corners = vorticityAtCorners(grid, u, v);
    ASSERT_EQ(corners.nx(), grid.nx + 1);
    ASSERT_EQ(corners.ny(), grid.ny + 1);
    for (int i = 0; i <= grid.nx; ++i) {
        for (int j = 0; j <= grid.ny; ++j) {
            EXPECT_NEAR(corners(i, j), 2.0, 1e-12) << "at corner " << i << ", " << j;
        }
    }
    const Field2D centers = vorticityField(grid, u, v);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_NEAR(centers(i, j), 2.0, 1e-12) << "at cell " << i << ", " << j;
        }
    }
    EXPECT_DOUBLE_EQ(divergenceField(grid, u, v).maxAbs(), 0.0);
}

// A shear layer u = (y, 0) has vorticity -1, which pins down the sign.
TEST(Diagnostics, VorticitySignFollowsTheShear) {
    const StaggeredGrid grid(4, 4, 1.0, 1.0);
    const Field2D u = sampleU(grid, [](double, double y) { return y; });
    const Field2D v = sampleV(grid, [](double, double) { return 0.0; });
    const Field2D centers = vorticityField(grid, u, v);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_NEAR(centers(i, j), -1.0, 1e-12);
        }
    }
}

TEST(Diagnostics, VelocityAtCentersAveragesTheOpposingFaces) {
    const StaggeredGrid grid(6, 3, 1.0, 1.0);
    const Field2D u = sampleU(grid, [](double x, double) { return x * x; });
    const Field2D v = sampleV(grid, [](double, double y) { return y * y; });

    Field2D uCenter;
    Field2D vCenter;
    velocityAtCenters(grid, u, v, &uCenter, &vCenter);
    const Field2D speed = speedField(grid, u, v);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_DOUBLE_EQ(uCenter(i, j), 0.5 * (u(i, j) + u(i + 1, j)));
            EXPECT_DOUBLE_EQ(vCenter(i, j), 0.5 * (v(i, j) + v(i, j + 1)));
            EXPECT_DOUBLE_EQ(speed(i, j), std::hypot(uCenter(i, j), vCenter(i, j)));
        }
    }

    // Either output may be omitted.
    Field2D onlyV;
    velocityAtCenters(grid, u, v, nullptr, &onlyV);
    EXPECT_DOUBLE_EQ(onlyV(2, 1), vCenter(2, 1));
}

TEST(Diagnostics, KineticEnergyOfAUniformFlowIsHalfTheSquareTimesTheArea) {
    const StaggeredGrid grid(10, 5, 2.0, 1.0);
    const Field2D u = sampleU(grid, [](double, double) { return 3.0; });
    const Field2D v = sampleV(grid, [](double, double) { return 4.0; });
    const double expected = 0.5 * (9.0 + 16.0) * grid.lengthX * grid.lengthY;
    EXPECT_NEAR(kineticEnergy(grid, u, v), expected, 1e-12);
}

TEST(Diagnostics, SolverAgreesWithTheFreeFunctions) {
    const StaggeredGrid grid(16, 16, 2.0 * M_PI, 2.0 * M_PI);
    SolverOptions options;
    options.viscosity = 0.05;
    NavierStokesSolver solver(grid, BoundaryConditions::doublyPeriodic(), options);
    solver.setVelocity([](double x, double y) { return -std::cos(x) * std::sin(y); },
                       [](double x, double y) { return std::sin(x) * std::cos(y); });
    solver.projectVelocity();
    for (int step = 0; step < 5; ++step) {
        solver.stepAdaptive(0.01);
    }

    EXPECT_NEAR(solver.kineticEnergy(), kineticEnergy(grid, solver.u(), solver.v()), 1e-12);
    EXPECT_NEAR(solver.maxDivergence(), divergenceField(grid, solver.u(), solver.v()).maxAbs(), 1e-15);

    Field2D uCenter;
    Field2D vCenter;
    velocityAtCenters(grid, solver.u(), solver.v(), &uCenter, &vCenter);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_DOUBLE_EQ(solver.uAtCenter(i, j), uCenter(i, j));
            EXPECT_DOUBLE_EQ(solver.vAtCenter(i, j), vCenter(i, j));
        }
    }

    // The Taylor-Green vortex is two counter-rotating pairs, so both signs of
    // vorticity have to be present.
    const Field2D vorticity = vorticityField(grid, solver.u(), solver.v());
    double lowest = 0.0;
    double highest = 0.0;
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            lowest = std::min(lowest, vorticity(i, j));
            highest = std::max(highest, vorticity(i, j));
        }
    }
    EXPECT_LT(lowest, -1.0);
    EXPECT_GT(highest, 1.0);
    EXPECT_NEAR(lowest, -highest, 1e-9);
}
