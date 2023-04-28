#include "../header.h"

class Solution {
public:
    int totalFruit(const vector<int>& fruits) {
        const int n = fruits.size();

        vector<pair<int, int>> f;
        int prev = -1;
        for (int i = 0; i < n; ++i) {
            if (fruits[i] != prev) {
                prev = fruits[i];
                f.emplace_back(make_pair(prev, i));
            }
        }
        const int m = f.size();
        f.emplace_back(make_pair(-1, n));

        int mx = 0;
        for (int ii = 0; ii < m; ++ii) {
            int i = f[ii].second;
            int j = ii;
            const int a = f[ii].first;
            while (j < m && f[j].first == a) {
                ++j;
            }
            if (j < m) {
                const int b = f[j].first;
                while (j < m && (f[j].first == a || f[j].first == b)) {
                    ++j;
                }
            }
            mx = max(mx, f[j].second - i);
        }
        return mx;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.totalFruit({1, 2, 1}) << endl;

    return 0;
}
