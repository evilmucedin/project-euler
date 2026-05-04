#include "../header.h"

#include <lib/unionFindDense.h>

class Solution {
public:
    int maxNumEdgesToRemove(int n, const vector<vector<int>>& edges) {
        UnionFindDense uf(n);
        int result = 0;

        for (const auto& e: edges) {
            if (e[0] != 3) {
                continue;
            }

            if (uf.find(e[1] - 1) != uf.find(e[2] - 1)) {
                uf.unite(e[1] - 1, e[2] - 1);
            } else {
                ++result;
            }
        }

        for (int i = 1; i < 3; ++i) {
            auto ufL = uf;
            for (const auto& e: edges) {
                if (e[0] != i) {
                    continue;
                }

                if (ufL.find(e[1] - 1) != ufL.find(e[2] - 1)) {
                    ufL.unite(e[1] - 1, e[2] - 1);
                } else {
                    ++result;
                }
            }

            if (ufL.components().size() != 1) {
                return -1;
            }
        }

        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxNumEdgesToRemove(4, {{3, 1, 2}, {3, 2, 3}, {1, 1, 4}, {2, 1, 4}}) << endl;

    return 0;
}
