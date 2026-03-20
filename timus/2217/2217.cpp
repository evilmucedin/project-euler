#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

using namespace std;

int main() {
    int x;
    string s;

    cin >> x >> s;

    int a = 0;
    int b = x - 1;
    while (a < b) {
        if (s[a] == s[b]) {
            ++a;
            --b;
        } else {
            if (s[a] < s[b]) {
                break;
            } else {
                int c = b - 1;
                while (a < c) {
                    if (s[a] == s[c]) {
                        std::swap(s[c], s[b]);
                        // sort(s.begin() + a, s.begin() + c + 1);
                        break;
                    } else {
                        --c;
                    }
                }

                if (s[a] != s[b])
                    break;
            }
        }
    }
    // cerr << a << " " << b << endl;

    bool can = (a >= b) || (x % 2 == 1 && a + 1 == b);

    if (can)
        cout << "YES" << endl;
    else
        cout << "NO" << endl;

    /*
    int x = 0;
    for (char ch: s) {
        if (ch == '0')
            --x;
        else if (ch == '1')
            ++x;
        else
            throw;
    }
    // cerr << a << " " << x << " " << (a % 2) << endl;

    if (x >= -1 && x <= 1) {
        if (a % 2 == 1)
            cout << "YES";
        else
            if (x == 0 && (a > 2))
                cout << "YES";
            else
                cout << "NO";
    } else {
        cout << "NO";
    }
    cout << endl;
    */

    return 0;
}
