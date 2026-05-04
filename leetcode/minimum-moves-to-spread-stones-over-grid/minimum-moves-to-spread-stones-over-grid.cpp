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
                    for (int k = 0; k + 1 < grid[i][j]; ++k) {
                        rich.emplace_back(Point{i, j});
                    }
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

        vector<IntVector> m(rich.size(), IntVector(bich.size()));
        for (int i = 0; i < rich.size(); ++i) {
            for (int j = 0; j < bich.size(); ++j) {
                m[i][j] = abs(rich[i].x - bich[j].x) + abs(rich[i].y - bich[j].y);
            }
        }

        return hungarian(m).first;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minimumMoves({{1, 1, 0}, {1, 1, 1}, {1, 2, 1}}) << endl;
    cerr << sol.minimumMoves({{1, 3, 0}, {1, 0, 0}, {1, 0, 3}}) << endl;

    return 0;
}
