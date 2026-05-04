#include "lib/header.h"

#include "lib/integrate.h"

int main() {
    auto ratio = [](int i) {
        double tgA = 1.0/i;
        double a = 1.0 + sqr(tgA);
        double b = -2.0*(1.0 + tgA);
        double c = 1;
        double x1 = (-b - sqrt(sqr(b) - 4.0*a*c))/2.0/a;

        auto f = [&](double x) {
            return 1.0 - sqrt(1.0 - sqr(x - 1.0));
        };

        double y1 = f(x1);

        double s1 = x1*y1/2.0;

        double s2 = integrate(f, x1, 1.0, 10000000);

        double s = s1 + s2;
        double ratio = s/(1.0 - M_PI/4.0);

        cout << OUT(i) << OUT(x1) << OUT(y1) << OUT(ratio) << endl;

        return ratio;
    };

    int l = 1;
    int r = 10000;
    while (r - l > 1) {
        int mid = (r + l) >> 1;
        double rt = ratio(mid);
        if (rt < 0.001) {
            r = mid;
        } else {
            l = mid;
        }
    }

    for (int i = l - 2; i <= l + 2; ++i) {
        cout << OUT(i) << OUT(ratio(i)) << endl;
    }

    return 0;
}
