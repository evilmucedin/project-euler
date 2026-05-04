#include <cstdio>

#include <iostream>
#include <vector>

using namespace std;

int main() {
    int n;
    cin >> n;

    bool has3 = false;
    bool only5 = true;
    int sum = 0;

    for (int i = 0; i < n; ++i) {
        int m;
        cin >> m;
        sum += m;
        if (m == 3) {
            has3 = true;
        }
        if (m != 5) {
            only5 = false;
        }
    }

    if (has3) {
        cout << "None" << endl;
    } else if (only5) {
        cout << "Named" << endl;
    } else if (2*sum >= 9*n) {
        cout << "High" << endl;
    } else {
        cout << "Common" << endl;
    }

    return 0;
}
