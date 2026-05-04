#include "../header.h"

#include "lib/unionFindDense.h"

class Solution {
public:
    int latestDayToCross(int row, int col, const vector<vector<int>>& cells) {
        UnionFindDense uf((row + 2)*col);

        auto idx = [&](int r, int c) {
            return row*col + c;
        };

        for (int i = 0; i < col; ++i) {
            uf.unite(idx(0, i), idx(1, i));
            uf.unite(idx(row, i), idx(row + 1, i));
        }

        vector<vector<bool>> map(row, vector<bool>(col));

        int day;
        for (day = cells.size() - 1; day >= 0; --day) {
            const auto& c = cells[day];
            map[c[0] - 1][c[1] - 1] = 1;
            static constexpr int DIRS[] = {-1, 0, 1, 0, 0, 1, 0, -1};
            for (int j = 0; j < 4; ++j) {
                int x = c[0] - 1 + DIRS[2*j];
                int y = c[1] - 1 + DIRS[2*j + 1];
                if (x >= 0 && x < row && y >= 0 && y < col) {
                    if (map[x][y]) {
                        uf.unite(idx(c[0], c[1] - 1), idx(x + 1, y));
                    }
                }
            }

            if (uf.find(idx(0, 0)) == uf.find(idx(row + 1, 0))) {
                break;
            }
        }
        return day;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.latestDayToCross(2, 2, {{1, 1}, {1, 2}, {2, 1}, {2, 2}}) << endl;
    cerr << sol.latestDayToCross(3, 3, {{1, 2}, {2, 1}, {3, 3}, {2, 2}, {1, 1}, {1, 3}, {2, 3}, {3, 2}, {3, 1}})
         << endl;

    return 0;
}
