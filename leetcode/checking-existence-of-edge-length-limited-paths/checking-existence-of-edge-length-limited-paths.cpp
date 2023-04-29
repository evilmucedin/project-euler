#include "../header.h"

#include <lib/unionFindDense.h>

class Solution {
public:
    struct Cmp {
        bool operator()(const vector<int>& a, const vector<int>& b) {
            return a[2] < b[2];
        }
    };

    vector<bool> distanceLimitedPathsExist(int n, const vector<vector<int>>& edgeList_, const vector<vector<int>>& queries_) {
        auto edgeList = edgeList_;
        std::sort(edgeList.begin(), edgeList.end(), Cmp());
        vector<vector<int>> queries(queries_.size());
        for (int i = 0; i < queries_.size(); ++i) {
            queries[i].resize(4);
            for (int j = 0; j < 3; ++j) {
                queries[i][j] = queries_[i][j];
            }
            queries[i][3] = i;
        }
        std::sort(queries.begin(), queries.end(), Cmp());

        UnionFindDense uf(n);
        vector<bool> result(queries.size());

        int j = 0;
        for (int i = 0; i < queries.size(); ++i) {
            while (j < edgeList.size() && edgeList[j][2] < queries[i][2]) {
                uf.unite(edgeList[j][0], edgeList[j][1]);
                ++j;
            }

            result[queries[i][3]] = (uf.find(queries[i][0]) == uf.find(queries[i][1]));
        }

        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.distanceLimitedPathsExist(3, {{0, 1, 2}, {1, 2, 4}, {2, 0, 8}, {1, 0, 16}}, {{0, 1, 2}, {0, 2, 5}})
         << endl;
    cerr << sol.distanceLimitedPathsExist(3, {{0, 1, 2}, {1, 2, 4}, {2, 0, 8}, {1, 0, 16}}, {{0, 1, 2}, {0, 2, 5}})
         << endl;
    cerr << sol.distanceLimitedPathsExist(5, {{0, 1, 10}, {1, 2, 5}, {2, 3, 9}, {3, 4, 13}}, {{0, 4, 14}, {1, 4, 13}})
         << endl;

    return 0;
}
