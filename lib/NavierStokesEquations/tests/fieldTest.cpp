#include "lib/NavierStokesEquations/field.h"

#include "gtest/gtest.h"

using namespace navierStokes;

TEST(Field2D, ShapeAndZeroInitialisation) {
    Field2D f(4, 3, 2);
    EXPECT_EQ(f.nx(), 4);
    EXPECT_EQ(f.ny(), 3);
    EXPECT_EQ(f.halo(), 2);
    for (int i = -2; i < 6; ++i) {
        for (int j = -2; j < 5; ++j) {
            EXPECT_EQ(f(i, j), 0.0);
        }
    }
}

TEST(Field2D, IndicesAreDistinct) {
    Field2D f(3, 4, 1);
    double next = 1.0;
    for (int i = -1; i <= 3; ++i) {
        for (int j = -1; j <= 4; ++j) {
            f(i, j) = next;
            next += 1.0;
        }
    }
    next = 1.0;
    for (int i = -1; i <= 3; ++i) {
        for (int j = -1; j <= 4; ++j) {
            EXPECT_EQ(f(i, j), next) << "at (" << i << ", " << j << ")";
            next += 1.0;
        }
    }
}

TEST(Field2D, ReductionsIgnoreGhostCells) {
    Field2D f(2, 2, 1);
    f(0, 0) = 1.0;
    f(1, 0) = 2.0;
    f(0, 1) = 3.0;
    f(1, 1) = -6.0;
    // Ghost cells hold values larger than anything in the interior.
    f(-1, -1) = 1000.0;
    f(2, 2) = -1000.0;
    f(-1, 1) = 500.0;

    EXPECT_DOUBLE_EQ(f.sum(), 0.0);
    EXPECT_DOUBLE_EQ(f.mean(), 0.0);
    EXPECT_DOUBLE_EQ(f.maxAbs(), 6.0);
}

TEST(Field2D, FillTouchesGhostCells) {
    Field2D f(2, 2, 1);
    f.fill(2.5);
    EXPECT_DOUBLE_EQ(f(-1, -1), 2.5);
    EXPECT_DOUBLE_EQ(f(1, 1), 2.5);
    EXPECT_DOUBLE_EQ(f.mean(), 2.5);
}

TEST(Field2D, SumIsAccurateForManyCancellingTerms) {
    // Plain summation of a large field of alternating large values loses the
    // small mean; the compensated sum in Field2D keeps it.
    Field2D f(100, 100, 0);
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            f(i, j) = ((i + j) % 2 == 0) ? 1e8 : -1e8;
        }
    }
    f(0, 0) = 1e8 + 1.0;
    EXPECT_NEAR(f.sum(), 1.0, 1e-9);
}

TEST(Field2D, SwapExchangesContents) {
    Field2D a(2, 2, 0);
    Field2D b(3, 1, 0);
    a(0, 0) = 7.0;
    b(0, 0) = -7.0;
    a.swap(b);
    EXPECT_EQ(a.nx(), 3);
    EXPECT_EQ(b.nx(), 2);
    EXPECT_DOUBLE_EQ(a(0, 0), -7.0);
    EXPECT_DOUBLE_EQ(b(0, 0), 7.0);
}
