#include "lib/NavierStokesEquations/obstacle.h"

#include <cmath>
#include <vector>

#include "gtest/gtest.h"

using namespace navierStokes;

namespace {

bool solidAt(const StaggeredGrid& grid, const std::vector<char>& solid, int i, int j) {
    return solid[static_cast<size_t>(i) * static_cast<size_t>(grid.ny) + static_cast<size_t>(j)] != 0;
}

}  // namespace

TEST(Obstacle, EmptyMaskHasTheRightShape) {
    const StaggeredGrid grid(7, 3, 1.0, 1.0);
    const std::vector<char> solid = solidNone(grid);
    EXPECT_EQ(solid.size(), static_cast<size_t>(grid.nx * grid.ny));
    EXPECT_EQ(solidCellCount(solid), 0);
}

// A cell is solid when its centre is inside the shape, so the mask is the
// staircase approximation of the disk and its area converges to pi r^2.
TEST(Obstacle, DiskAreaConvergesToTheAnalyticValue) {
    const double radius = 0.2;
    const double exactArea = M_PI * radius * radius;
    double previousError = 0.0;
    for (int n : {32, 64, 128}) {
        const StaggeredGrid grid(n, n, 1.0, 1.0);
        const std::vector<char> solid = solidDisk(grid, 0.5, 0.5, radius);
        const double area = solidCellCount(solid) * grid.cellArea();
        const double error = std::fabs(area - exactArea);
        EXPECT_LT(error / exactArea, 0.1) << "at n = " << n;
        if (previousError > 0.0) {
            EXPECT_LT(error, previousError + 1e-12) << "at n = " << n;
        }
        previousError = error;
    }
}

TEST(Obstacle, DiskIsCenteredAndBounded) {
    const StaggeredGrid grid(40, 40, 1.0, 1.0);
    const std::vector<char> solid = solidDisk(grid, 0.5, 0.5, 0.15);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const double distance = std::hypot(grid.xP(i) - 0.5, grid.yP(j) - 0.5);
            EXPECT_EQ(solidAt(grid, solid, i, j), distance <= 0.15) << "at " << i << ", " << j;
        }
    }
    // The disk does not reach the domain edge, which is what lets it be used
    // with an inflow / outflow configuration.
    for (int j = 0; j < grid.ny; ++j) {
        EXPECT_FALSE(solidAt(grid, solid, 0, j));
        EXPECT_FALSE(solidAt(grid, solid, grid.nx - 1, j));
    }
}

TEST(Obstacle, ZeroRadiusDiskMarksNothing) {
    const StaggeredGrid grid(8, 8, 1.0, 1.0);
    // No cell centre lies exactly on a corner of the grid, so a degenerate
    // disk there marks no cells at all.
    EXPECT_EQ(solidCellCount(solidDisk(grid, 0.0, 0.0, 0.0)), 0);
}

TEST(Obstacle, RectangleCoversTheCellsWhoseCentresItContains) {
    const StaggeredGrid grid(10, 10, 1.0, 1.0);  // dx = dy = 0.1, centres at 0.05, 0.15, ...
    const std::vector<char> solid = solidRectangle(grid, 0.2, 0.4, 0.5, 0.6);
    // Centres inside [0.2, 0.5] along x are 0.25, 0.35, 0.45 -> i = 2, 3, 4.
    // Centres inside [0.4, 0.6] along y are 0.45, 0.55 -> j = 4, 5.
    EXPECT_EQ(solidCellCount(solid), 6);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const bool expected = i >= 2 && i <= 4 && j >= 4 && j <= 5;
            EXPECT_EQ(solidAt(grid, solid, i, j), expected) << "at " << i << ", " << j;
        }
    }
}

TEST(Obstacle, UnionMergesShapesAndCountsOverlapOnce) {
    const StaggeredGrid grid(20, 20, 1.0, 1.0);
    std::vector<char> left = solidRectangle(grid, 0.0, 0.0, 0.5, 0.5);
    const std::vector<char> right = solidRectangle(grid, 0.25, 0.25, 0.75, 0.75);
    const int leftCount = solidCellCount(left);
    const int rightCount = solidCellCount(right);
    const int overlap = solidCellCount(solidRectangle(grid, 0.25, 0.25, 0.5, 0.5));
    ASSERT_GT(overlap, 0);

    solidUnion(&left, right);
    EXPECT_EQ(solidCellCount(left), leftCount + rightCount - overlap);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_LE(left[static_cast<size_t>(i) * grid.ny + j], 1);
        }
    }
}

TEST(Obstacle, UnionWithNothingChangesNothing) {
    const StaggeredGrid grid(6, 6, 1.0, 1.0);
    std::vector<char> disk = solidDisk(grid, 0.5, 0.5, 0.2);
    const std::vector<char> before = disk;
    solidUnion(&disk, solidNone(grid));
    EXPECT_EQ(disk, before);
}

TEST(Obstacle, MaskAsFieldIsAnIndicator) {
    const StaggeredGrid grid(12, 9, 1.2, 0.9);
    const std::vector<char> solid = solidDisk(grid, 0.6, 0.45, 0.2);
    const Field2D field = solidAsField(grid, solid);
    ASSERT_EQ(field.nx(), grid.nx);
    ASSERT_EQ(field.ny(), grid.ny);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            EXPECT_DOUBLE_EQ(field(i, j), solidAt(grid, solid, i, j) ? 1.0 : 0.0);
        }
    }
    EXPECT_DOUBLE_EQ(field.sum(), solidCellCount(solid));
}

TEST(Obstacle, EmptyMaskAsFieldIsAllFluid) {
    const StaggeredGrid grid(5, 5, 1.0, 1.0);
    const Field2D field = solidAsField(grid, std::vector<char>());
    EXPECT_DOUBLE_EQ(field.maxAbs(), 0.0);
}
