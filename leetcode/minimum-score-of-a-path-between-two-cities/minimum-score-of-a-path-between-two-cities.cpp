#include "../header.h"

#include <lib/unionFindDense.h>

class Solution {
public:
    int minScore(int n, const vector<vector<int>>& roads) {
        UnionFindDense uf(n);
        for (const auto& e: roads) {
            uf.unite(e[0] - 1, e[1] - 1);
        }
        int mn = roads[0][2];
        for (const auto& e: roads) {
            if (uf.find(e[0] - 1) == uf.find(0) && uf.find(e[1] - 1) == uf.find(0)) {
                mn = min(mn, e[2]);
            }
        }

        return mn;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minScore(4, {{1, 2, 9}, {2, 3, 6}, {2, 4, 5}, {1, 4, 7}}) << endl;
    cerr << sol.minScore(6, {{4, 5, 7468},
                             {6, 2, 7173},
                             {6, 3, 8365},
                             {2, 3, 7674},
                             {5, 6, 7852},
                             {1, 2, 8547},
                             {2, 4, 1885},
                             {2, 5, 5192},
                             {1, 3, 4065},
                             {1, 4, 7357}})
         << endl;

    return 0;
}
