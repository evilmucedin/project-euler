#include <iostream>

#include "lib/math.h"

#include "gtest/gtest.h"

using namespace std;

TEST(RK4, Simple) {
    // Test the rk4 function with a few values of t, y0, dydt, and dt
    const vector<double> tt = {0.0, 1.0, 2.0, 3.0};
    const double y0 = 1.0; // Initial condition y0 = 1 at t = 0
    const vector<double> dydt = {1.5, -0.5, 0.25}; // First derivative dydt
    const vector<double> dt = {0.1}; // Time step

    for (double t : tt)
    {
        cout << "t: " << t << ", y0: " << y0 << ", dydt: " << dydt[0];
        double result = rungeKutta4(t, y0, dydt, dt);
        cout << ", Result: " << result;
        cout << endl;
        EXPECT_NEAR(result, 1.0625, 1e-9);
    }
}
