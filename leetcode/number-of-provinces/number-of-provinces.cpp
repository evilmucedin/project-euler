#include "../header.h"

#include "lib/unionFindDense.h"

class Solution {
public:
    int findCircleNum(const vector<vector<int>>& isConnected) {
        const int n = isConnected.size();
        UnionFindDense g(n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (isConnected[i][j]) {
                    g.unite(i, j);
                }
            }
        }

        return g.components().size();
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findCircleNum({{1,1,0},{1,1,0},{0,0,1}}) << endl;
    cerr << sol.findCircleNum({{1,0,0},{0,1,0},{0,0,1}}) << endl;

    return 0;
}
