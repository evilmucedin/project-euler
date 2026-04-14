#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

string pi(int len) {
    if (len <= 0) return "3";

    double p = 4.0 * (4.0 / 8.0 + 2.0 / 16.0 + 1.0 / 4.0);
    if (len == 1) return to_string(p);

    int n = len - 1;
    double pi_val = p;

    for (int i = 1; i <= n; ++i) {
        double term = pow(16, -i) * (
            4.0 / (8.0 * i + 1.0) -
            2.0 / (8.0 * i + 4.0) -
            1.0 / (8.0 * i + 5.0) -
            1.0 / (8.0 * i + 6.0)
        );
        pi_val += term;
    }

    ostringstream oss;
    oss << fixed << setprecision(len - 2) << pi_val;
    return "3." + oss.str();
}

int main() {
    cout << pi(15) << endl;
    cout << pi(100) << endl;
    return 0;
}

