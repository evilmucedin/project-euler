#include "../header.h"

class Solution {
public:
    vector<int> spiralOrder(const vector<vector<int>>& matrix_) {
        auto matrix = matrix_;
        static constexpr int INF = -123456789;
        int x = 0;
        int y = 0;
        int dir = 0;
        static constexpr int DIRS[] = {0, 1, 1, 0, 0, -1, -1, 0};

        auto good = [&](int x, int y, int dir) {
            x += DIRS[2 * dir];
            y += DIRS[2 * dir + 1];
            if (x < 0 || x >= matrix.size()) {
                return false;
            }
            if (y < 0 || y >= matrix[x].size()) {
                return false;
            }
            return matrix[x][y] != INF;
        };

        vector<int> result;
        while (matrix[x][y] != INF) {
            result.emplace_back(matrix[x][y]);
            matrix[x][y] = INF;
            for (int k = 0; k < 4; ++k) {
                if (good(x, y, (dir + k) % 4)) {
                    dir = (dir + k) % 4;
                    x += DIRS[2*dir];
                    y += DIRS[2*dir + 1];
                    break;
                }
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.spiralOrder({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}) << endl;

    return 0;
}
