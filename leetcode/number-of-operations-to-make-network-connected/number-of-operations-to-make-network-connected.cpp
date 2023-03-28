#include <lib/unionFindDense.h>

#include "../header.h"

class Solution {
   public:
    int makeConnected(int n, const vector<vector<int>>& connections) {
        if (connections.size() + 1 < n) {
            return -1;
        }

        UnionFindDense uf(n);
        for (const auto& e : connections) {
            uf.unite(e[0], e[1]);
        }

        const auto comps = uf.components();
        if (comps.size() == 1) {
            return 0;
        }

        return comps.size() - 1;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.makeConnected(4, {{0, 1}, {0, 2}, {1, 2}}) << endl;

    return 0;
}
