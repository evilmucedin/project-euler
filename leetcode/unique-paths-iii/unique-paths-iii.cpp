#include "../header.h"

class Solution {
public:
    using Point = pair<int, int>;

    Point find(const vector<vector<int>>& grid, int value) {
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                if (grid[i][j] == value) {
                    return {i, j};
                }
            }
        }
        return {-1, -1};
    }

    void rec(const Point& now, const vector<vector<int>>& grid, vector<vector<bool>>& visited, vector<vector<int>>& counts) {
        if (now.first < 0 && now.first >= grid.size()) {
            return;
        }
        if (now.second < 0 && now.second >= grid[0].size()) {
            return;
        }
        if (grid[now.first][now.second] == -1) {
            return;
        }

        if (!visited[now.first][now.second]) {
            visited[now.first][now.second] = true;
            ++counts[now.first][now.second];
            static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1};
            for (int i = 0; i < 4; ++i) {
                Point next = now;
                next.first += DIRS[2*i];
                next.second += DIRS[2*i + 1];
                rec(next, grid, visited, counts);
            }
            visited[now.first][now.second] = false;
        }
    }

    int uniquePathsIII(const vector<vector<int>>& grid) {
        auto start = find(grid, 1);
        auto finish = find(grid, 2);
        const int n = grid.size();
        const int m = grid[0].size();
        vector<vector<bool>> visited(n, vector<bool>(m));
        vector<vector<int>> counts(n, vector<int>(m));
        rec(start, grid, visited, counts);
        return counts[finish.first][finish.second];
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.uniquePathsIII({{1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 2, -1}}) << endl;

    return 0;
}
