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
    vector<int> a;
    for (int i = 0; i < x; ++i)
        if (s[i] == '0')
            a.push_back(i+1); // 1-based positions

    int k = (int)a.size();
    int n = x;
    int h = n/2;

    bool possible = false;
    for (int mid = 0; mid <= (n%2); ++mid) {
        if ((k - mid) < 0) continue;
        if ((k - mid) % 2 != 0) continue;
        int m = (k - mid)/2;
        if (m > h) continue;
        if (mid == 1) {
            if (m >= (int)a.size()) continue;
            if (h+1 > a[m]) continue;
        }

        // upper bounds ub[s] = A[s] for s=0..m-1
        // lower bounds lb[s] = n - A[2*m+mid - s - 1] + 1
        vector<int> ub(m), lb(m);
        for (int sidx = 0; sidx < m; ++sidx) ub[sidx] = a[sidx];
        for (int sidx = 1; sidx <= m; ++sidx) {
            int idx = 2*m + mid - sidx; // 0-based
            lb[sidx-1] = max(1, n - a[idx] + 1);
        }

        int cur = 1;
        bool ok = true;
        for (int sidx = 0; sidx < m; ++sidx) {
            int need = max(lb[sidx], cur);
            if (need > ub[sidx]) { ok = false; break; }
            cur = need + 1;
        }
        if (ok) { possible = true; break; }
    }

    cout << (possible ? "YES" : "NO") << endl;

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
