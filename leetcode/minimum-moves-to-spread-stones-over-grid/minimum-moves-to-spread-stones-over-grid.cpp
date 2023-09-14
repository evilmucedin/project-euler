#include "../header.h"

#include "lib/graph/hungarian.h"
#include "lib/geometry.h"

class Solution {
public:
    using Point = Point2<int>;
    using Points = vector<Point>;

    int minimumMoves(const vector<vector<int>>& grid) {
        Points rich;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (grid[i][j] > 1) {
                    rich.emplace_back(Point{i, j});
                }
            }
        }
        Points bich;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (grid[i][j] == 0) {
                    bich.emplace_back(Point{i, j});
                }
            }
        }
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minimumMoves({{1, 1, 0}, {1, 1, 1}, {1, 2, 1}}) << endl;

    return 0;
}
