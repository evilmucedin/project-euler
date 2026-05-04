#include "gtest/gtest.h"

#include "lib/integrate.h"

TEST(Integrate, Parabola) {
    auto parabola = [](double x) { return x * x; };
    double ans = integrate(parabola, 0.0, 1.0, 1000000);
    EXPECT_NEAR(ans, 1.0 / 3, 1e-8);
}
