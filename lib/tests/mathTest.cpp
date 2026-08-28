#include <cmath>

#include "lib/math.h"

#include "gtest/gtest.h"

using namespace std;

// A single RK4 step for y' = y from y(0) = 1 must reproduce the degree-4
// Taylor polynomial of e^h exactly: 1 + h + h^2/2 + h^3/6 + h^4/24.
TEST(RK4, StepMatchesTaylorPolynomial) {
    auto f = [](double, double y) { return y; };
    const double h = 0.5;
    const double expected = 1 + h + h*h/2 + h*h*h/6 + h*h*h*h/24;
    EXPECT_NEAR(rungeKutta4Step(f, 0.0, 1.0, h), expected, 1e-15);
}

// When f depends only on t, RK4 reduces to Simpson's rule, which is exact
// for polynomials up to degree 3 even with large steps.
TEST(RK4, ExactOnCubicPolynomial) {
    auto f = [](double t, double) { return 3*t*t - 2*t + 1; };
    // y(t) = t^3 - t^2 + t, y(0) = 0
    EXPECT_NEAR(rungeKutta4(f, 0.0, 0.0, 2.0, 2), 6.0, 1e-12);
    EXPECT_NEAR(rungeKutta4(f, 0.0, 0.0, 5.0, 1), 105.0, 1e-12);
}

// y' = y, y(0) = 1  =>  y(t) = e^t.
TEST(RK4, Exponential) {
    auto f = [](double, double y) { return y; };
    EXPECT_NEAR(rungeKutta4(f, 0.0, 1.0, 1.0, 100), exp(1.0), 1e-9);
    EXPECT_NEAR(rungeKutta4(f, 0.0, 1.0, 3.0, 300), exp(3.0), 1e-7);
}

// Integrating backwards from y(1) = e must recover y(0) = 1.
TEST(RK4, BackwardIntegration) {
    auto f = [](double, double y) { return y; };
    EXPECT_NEAR(rungeKutta4(f, 1.0, exp(1.0), 0.0, 100), 1.0, 1e-9);
}

// Non-autonomous equation: y' = -2ty, y(0) = 1  =>  y(t) = e^(-t^2).
TEST(RK4, Gaussian) {
    auto f = [](double t, double y) { return -2 * t * y; };
    EXPECT_NEAR(rungeKutta4(f, 0.0, 1.0, 1.0, 100), exp(-1.0), 1e-9);
    EXPECT_NEAR(rungeKutta4(f, 0.0, 1.0, 2.0, 200), exp(-4.0), 1e-9);
}

// Logistic equation: y' = y(1 - y), y(0) = 1/2  =>  y(t) = 1/(1 + e^-t).
TEST(RK4, Logistic) {
    auto f = [](double, double y) { return y * (1 - y); };
    EXPECT_NEAR(rungeKutta4(f, 0.0, 0.5, 4.0, 200), 1 / (1 + exp(-4.0)), 1e-10);
}

// Halving the step size must shrink the global error ~16x (4th order).
TEST(RK4, FourthOrderConvergence) {
    auto f = [](double t, double y) { return -2 * t * y; };
    const double exact = exp(-1.0);
    const double errCoarse = fabs(rungeKutta4(f, 0.0, 1.0, 1.0, 10) - exact);
    const double errFine = fabs(rungeKutta4(f, 0.0, 1.0, 1.0, 20) - exact);
    const double order = log2(errCoarse / errFine);
    EXPECT_GT(errCoarse, 0.0);
    EXPECT_NEAR(order, 4.0, 0.3);
}
