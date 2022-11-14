#include "../header.h"

#include <lib/unionFind.h>

class Solution {
public:
    int removeStones(const vector<vector<int>>& stones) {
        UnionFind<int> uf;
        const int n = stones.size();
        for (int i = 0; i < n; ++i) {
            uf.add(i);
        }
        for (int i = 0; i < stones.size(); ++i) {
            for (int j = i + 1; j < stones.size(); ++j) {
                if (stones[i][0] == stones[j][0] || stones[i][1] == stones[j][1]) {
                    uf.unite(i, j);
                }
            }
        }

        return n - uf.components().size();
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.removeStones({{0,0},{0,1},{1,0},{1,2},{2,1},{2,2}}) << endl;

    return 0;
}
