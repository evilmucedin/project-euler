#include "lib/NavierStokesEquations/boundary.h"

#include <cmath>

#include "gtest/gtest.h"

using namespace navierStokes;

namespace {

struct VelocityFields {
    VelocityFields(const StaggeredGrid& grid) : u(grid.nx + 1, grid.ny, 1), v(grid.nx, grid.ny + 1, 1) {}
    Field2D u;
    Field2D v;
};

// Fills the interior with a pattern that is different in every cell, so that a
// boundary rule copying from the wrong place is visible.
void fillDistinct(const StaggeredGrid& grid, VelocityFields* fields) {
    for (int i = 0; i <= grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            fields->u(i, j) = 1.0 + i + 100.0 * j;
        }
    }
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j <= grid.ny; ++j) {
            fields->v(i, j) = -1.0 - i - 100.0 * j;
        }
    }
}

}  // namespace

TEST(Boundary, NoSlipWallsStopNormalFlow) {
    StaggeredGrid grid(4, 3, 1.0, 1.0);
    VelocityFields fields(grid);
    fillDistinct(grid, &fields);

    applyVelocityBoundaries(grid, BoundaryConditions::cavity(0.0), fields.u, fields.v);

    for (int j = 0; j < grid.ny; ++j) {
        EXPECT_DOUBLE_EQ(fields.u(0, j), 0.0);
        EXPECT_DOUBLE_EQ(fields.u(grid.nx, j), 0.0);
        // The tangential component averages to zero on the wall itself.
        EXPECT_DOUBLE_EQ(0.5 * (fields.v(-1, j) + fields.v(0, j)), 0.0);
        EXPECT_DOUBLE_EQ(0.5 * (fields.v(grid.nx, j) + fields.v(grid.nx - 1, j)), 0.0);
    }
    for (int i = 0; i < grid.nx; ++i) {
        EXPECT_DOUBLE_EQ(fields.v(i, 0), 0.0);
        EXPECT_DOUBLE_EQ(fields.v(i, grid.ny), 0.0);
        EXPECT_DOUBLE_EQ(0.5 * (fields.u(i, -1) + fields.u(i, 0)), 0.0);
        EXPECT_DOUBLE_EQ(0.5 * (fields.u(i, grid.ny) + fields.u(i, grid.ny - 1)), 0.0);
    }
}

TEST(Boundary, MovingLidAppearsOnTheWall) {
    StaggeredGrid grid(4, 4, 1.0, 1.0);
    VelocityFields fields(grid);
    fillDistinct(grid, &fields);

    applyVelocityBoundaries(grid, BoundaryConditions::cavity(1.5), fields.u, fields.v);

    for (int i = 0; i <= grid.nx; ++i) {
        // The lid value is what the interpolation to the wall returns, which is
        // the only place the tangential velocity is actually defined.
        EXPECT_DOUBLE_EQ(0.5 * (fields.u(i, grid.ny) + fields.u(i, grid.ny - 1)), 1.5);
    }
    for (int i = 0; i < grid.nx; ++i) {
        EXPECT_DOUBLE_EQ(fields.v(i, grid.ny), 0.0);
    }
}

TEST(Boundary, FreeSlipLeavesTangentialShearAtZero) {
    StaggeredGrid grid(4, 4, 1.0, 1.0);
    VelocityFields fields(grid);
    fillDistinct(grid, &fields);

    BoundaryConditions bc;
    bc.bottom.kind = BoundaryKind::FreeSlip;
    bc.top.kind = BoundaryKind::FreeSlip;
    applyVelocityBoundaries(grid, bc, fields.u, fields.v);

    for (int i = 0; i <= grid.nx; ++i) {
        EXPECT_DOUBLE_EQ(fields.u(i, -1), fields.u(i, 0));
        EXPECT_DOUBLE_EQ(fields.u(i, grid.ny), fields.u(i, grid.ny - 1));
    }
}

TEST(Boundary, PeriodicWrapsBothComponents) {
    StaggeredGrid grid(5, 4, 1.0, 1.0);
    VelocityFields fields(grid);
    fillDistinct(grid, &fields);

    applyVelocityBoundaries(grid, BoundaryConditions::doublyPeriodic(), fields.u, fields.v);

    for (int j = 0; j < grid.ny; ++j) {
        EXPECT_DOUBLE_EQ(fields.u(grid.nx, j), fields.u(0, j));
        EXPECT_DOUBLE_EQ(fields.u(-1, j), fields.u(grid.nx - 1, j));
    }
    for (int j = 0; j <= grid.ny; ++j) {
        EXPECT_DOUBLE_EQ(fields.v(-1, j), fields.v(grid.nx - 1, j));
        EXPECT_DOUBLE_EQ(fields.v(grid.nx, j), fields.v(0, j));
    }
    for (int i = 0; i < grid.nx; ++i) {
        EXPECT_DOUBLE_EQ(fields.v(i, grid.ny), fields.v(i, 0));
        EXPECT_DOUBLE_EQ(fields.v(i, -1), fields.v(i, grid.ny - 1));
    }
    for (int i = 0; i <= grid.nx; ++i) {
        EXPECT_DOUBLE_EQ(fields.u(i, -1), fields.u(i, grid.ny - 1));
        EXPECT_DOUBLE_EQ(fields.u(i, grid.ny), fields.u(i, 0));
    }
}

TEST(Boundary, PeriodicBoundariesCarryNoNetFlux) {
    StaggeredGrid grid(5, 4, 2.0, 1.0);
    VelocityFields fields(grid);
    fillDistinct(grid, &fields);
    applyVelocityBoundaries(grid, BoundaryConditions::doublyPeriodic(), fields.u, fields.v);
    EXPECT_NEAR(netBoundaryFlux(grid, fields.u, fields.v), 0.0, 1e-12);
}

TEST(Boundary, InflowIsPrescribedAndOutflowBalancesIt) {
    StaggeredGrid grid(6, 4, 3.0, 1.0);
    VelocityFields fields(grid);
    fillDistinct(grid, &fields);

    const double inflow = 2.0;
    applyVelocityBoundaries(grid, BoundaryConditions::inflowOutflow(inflow), fields.u, fields.v);

    for (int j = 0; j < grid.ny; ++j) {
        EXPECT_DOUBLE_EQ(fields.u(0, j), inflow);
    }
    // The outflow faces are rescaled so that as much fluid leaves as enters,
    // which is what makes the Neumann pressure problem solvable.
    EXPECT_NEAR(netBoundaryFlux(grid, fields.u, fields.v), 0.0, 1e-12);

    double outflux = 0.0;
    for (int j = 0; j < grid.ny; ++j) {
        outflux += fields.u(grid.nx, j) * grid.dy;
    }
    EXPECT_NEAR(outflux, inflow * grid.lengthY, 1e-12);
}

TEST(Boundary, OutflowKeepsTheShapeOfTheProfile) {
    StaggeredGrid grid(6, 4, 3.0, 1.0);
    VelocityFields fields(grid);
    // A non-uniform profile leaving the domain: the correction is a uniform
    // shift, so differences between faces must survive it.
    for (int i = 0; i <= grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            fields.u(i, j) = 1.0 + 0.25 * j;
        }
    }
    applyVelocityBoundaries(grid, BoundaryConditions::inflowOutflow(1.0), fields.u, fields.v);
    for (int j = 1; j < grid.ny; ++j) {
        EXPECT_NEAR(fields.u(grid.nx, j) - fields.u(grid.nx, j - 1), 0.25, 1e-12);
    }
}

TEST(Boundary, PeriodicFlagsRequireBothSidesToAgree) {
    BoundaryConditions bc = BoundaryConditions::periodicChannel();
    EXPECT_TRUE(bc.periodicX());
    EXPECT_FALSE(bc.periodicY());

    EXPECT_FALSE(BoundaryConditions::cavity(1.0).periodicX());
    EXPECT_TRUE(BoundaryConditions::doublyPeriodic().periodicY());
}
