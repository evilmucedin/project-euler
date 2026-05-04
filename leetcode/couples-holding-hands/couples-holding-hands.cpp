#include "../header.h"

#include <lib/unionFind.h>

class Solution {
public:
    int minSwapsCouples(const vector<int>& row) {
        UnionFind<int> uf;
        const int n = row.size() / 2;
        for (int i = 0; i < n; ++i) {
            uf.add(i);
        }
        for (int i = 0; i < n; ++i) {
            uf.unite(row[2*i] / 2, row[2*i + 1] / 2);
        }

        return n - uf.components().size();
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minSwapsCouples({0, 2, 1, 3}) << endl;
    cerr << sol.minSwapsCouples({3, 2, 0, 1}) << endl;

    return 0;
}
