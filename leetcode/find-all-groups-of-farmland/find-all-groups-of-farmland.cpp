#include "../header.h"

class Solution {
public:
    using Point = pair<int, int>;
    using Points = unordered_set<Point>;

    void findIsland(const vector<vector<int>>& land, Point& points, int x, int y) {

    }

    vector<vector<int>> findFarmland(const vector<vector<int>>& land) {
        const int n = land.size();
        const int m = land[0].size();
        Points pLand;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (land[i][j] == 1) {
                    findIsland(land, pLand, i, j);
                }
            }
        }
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findFarmland({{1, 0, 0}, {0, 1, 1}, {0, 1, 1}}) << endl;

    return 0;
}
