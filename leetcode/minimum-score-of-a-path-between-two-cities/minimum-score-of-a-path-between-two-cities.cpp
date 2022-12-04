#include "../header.h"
#include "lib/unionFindDense.h"

class Solution {
public:
    int minScore(int n, const vector<vector<int>>& roads) {
        const int INF = 1000000000;
        int mn = INF;
        UnionFindDense uf(n);
        for (const auto& r: roads) {
            uf.unite(r[0] - 1, r[1] - 1);
        }
        for (const auto& r: roads) {
            if (uf.find(r[0] - 1) == uf.find(0)) {
                mn = min(mn, r[2]);
            }
        }
        return mn;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minScore(4, {{1, 2, 9}, {2, 3, 6}, {2, 4, 5}, {1, 4, 7}}) << endl;
    cerr << sol.minScore(4, {{1, 2, 2}, {1, 3, 4}, {3, 4, 7}}) << endl;

    return 0;
}
