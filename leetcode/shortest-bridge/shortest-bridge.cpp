#include "../header.h"

class Solution {
   public:
    void mark(vector<vector<int>>& grid, int i, int j, int index) {
        if (i < 0 || i >= grid.size()) {
            return;
        }
        if (j < 0 || j >= grid[i].size()) {
            return;
        }

        if (grid[i][j] == 1) {
            grid[i][j] = index;
            mark(grid, i - 1, j, index);
            mark(grid, i + 1, j, index);
            mark(grid, i, j - 1, index);
            mark(grid, i, j + 1, index);
        }
    }

    int shortestBridge(vector<vector<int>>& grid) {
        auto good = [&](int i, int j) {
            if (i < 0 || i >= grid.size()) {
                return false;
            }
            if (j < 0 || j >= grid[i].size()) {
                return false;
            }
            return true;
        };

        auto find = [&](int index) {
            for (int i = 0; i < grid.size(); ++i) {
                for (int j = 0; j < grid[i].size(); ++j) {
                    if (grid[i][j] == 1) {
                        mark(grid, i, j, index);
                        return;
                    }
                }
            }
        };

        find(2);
        find(3);

        using PII = tuple<int, int, int>;
        queue<PII> q;
        vector<vector<bool>> visited(grid.size(), vector<bool>(grid[0].size()));

        int mn = 123456789;
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                if (grid[i][j] == 2) {
                    q.emplace(make_tuple(i, j, 0));
                    visited[i][j] = true;
                }
            }
        }
        while (!q.empty()) {
            const auto now = q.front();
            q.pop();
            int x = get<0>(now);
            int y = get<1>(now);
            visited[x][y] = true;
            if (grid[x][y] == 3) {
                mn = min(mn, get<2>(now));
                break;
            }
            static constexpr int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1};
            for (int k = 0; k < 4; ++k) {
                int nx = x + DIRS[2 * k];
                int ny = y + DIRS[2 * k + 1];
                if (good(nx, ny)) {
                    if (!visited[nx][ny]) {
                        q.emplace(make_tuple(x + DIRS[2 * k], y + DIRS[2 * k + 1], get<2>(now) + 1));
                    }
                }
            }
        }

        return mn - 1;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    // cerr << sol.<< endl;

    return 0;
}
