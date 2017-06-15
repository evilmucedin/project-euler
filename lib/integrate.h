#pragma once

template<typename T>
double integrate(T& f, double a, double b, int n) {
    double dx = (b - a)/n;
    double result = 0.0;
    result += f(a)/2.0;
    result += f(b)/2.0;
    double x = a;
    for (int k = 1; k < n; ++k) {
        x += dx;
        result += f(x);
    }
    return result*dx;
}
