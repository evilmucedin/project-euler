#include "../header.h"

class Solution {
public:
    using Point = pair<int, int>;
    using Points = set<Point>;

    void findIsland(const vector<vector<int>>& land, Points& points, int x, int y) {
        Point p(x, y);
        if (points.count(p) > 0) {
            return;
        }
        if (x < 0 || y < 0) {
            return;
        }
        if (x >= land.size() || y >= land[x].size()) {
            return;
        }
        if (land[x][y]) {
            points.insert(p);
            findIsland(land, points, x - 1, y);
            findIsland(land, points, x + 1, y);
            findIsland(land, points, x, y - 1);
            findIsland(land, points, x, y + 1);
        }
    }

    vector<vector<int>> decomposeIntoRects(Points& land) {
        vector<vector<int>> rects;
        int n = land.size(), m = land[0].size();
        vector<vector<bool>> visited(n, vector<bool>(m, false));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (land[i][j] == 1 && !visited[i][j]) {
                    int minX = i, minY = j, maxX = i, maxY = j;
                    queue<Point> q;
                    q.push({i, j});
                    visited[i][j] = true;

                    while (!q.empty()) {
                        Point p = q.front();
                        q.pop();
                        int x = p.first, y = p.second;
                        minX = min(minX, x);
                        minY = min(minY, y);
                        maxX = max(maxX, x);
                        maxY = max(maxY, y);

                        // Check all four directions
                        vector<Point> directions = {{x - 1, y}, {x + 1, y}, {x, y - 1}, {x, y + 1}};
                        for (const Point& dir : directions) {
                            int newX = dir.first, newY = dir.second;
                            if (newX >= 0 && newY >= 0 && newX < n && newY < m && land[newX][newY] == 1 && !visited[newX][newY]) {
                                q.push({newX, newY});
                                visited[newX][newY] = true;
                            }
                        }
                    }
                    rects.push_back({minX, minY, maxX, maxY});
                }
            }
        }
        return rects;
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

        return decomposeIntoRects(pLand);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findFarmland({{1, 0, 0}, {0, 1, 1}, {0, 1, 1}}) << endl;

    return 0;
}
