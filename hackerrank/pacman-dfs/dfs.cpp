#include <iostream>
#include <vector>

#include <stack>

using namespace std;

struct DFSCell {
    int parentR{-1};
    int parentC{-1};
    bool visited{};
};

using DFSMatrix = vector<vector<DFSCell>>;

void print(const DFSMatrix& m, int x, int y, int len, const vector<string>& grid) {
    if (grid[x][y] == 'P') {
        cout << len << endl;
    }
    const auto& c = m[x][y];
    if (c.parentR != -1) {
        print(m, c.parentR, c.parentC, len + 1, grid);
    }
    cout << x << " " << y << endl;
}

void dfs(int r, int c, int pacman_r, int pacman_c, int food_r, int food_c, vector<string> grid) {
    DFSMatrix m(r, vector<DFSCell>(c));
    stack<pair<int, int>> q;
    q.emplace(pacman_r, pacman_c);
    size_t count = 1;
    m[pacman_r][pacman_c].visited = true;
    vector<pair<int, int>> e;
    while (!q.empty()) {
        auto now = q.top();
        e.emplace_back(now);
        q.pop();
        ++count;

        if (now.first == food_r && now.second == food_c) {
            break;
        }

        static const int DIRS[][2] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};
        for (int k = 0; k < 4; ++k) {
            int x = now.first + DIRS[k][0];
            int y = now.second + DIRS[k][1];
            if (x < 0 || x >= r) {
                continue;
            }
            if (y < 0 || y >= c) {
                continue;
            }
            if (grid[x][y] != '-' && grid[x][y] != 'P' && grid[x][y] != '.') {
                continue;
            }
            auto& cc = m[x][y];
            if (cc.visited) {
                continue;
            }
            cc.visited = true;
            cc.parentR = now.first;
            cc.parentC = now.second;
            q.emplace(x, y);
        }
    }
    cout << e.size() << endl;
    for (const auto& c: e) {
        cout << c.first << " " << c.second << endl;
    }
    print(m, food_r, food_c, 0, grid);
}

int main(void) {

    int r,c, pacman_r, pacman_c, food_r, food_c;

    cin >> pacman_r >> pacman_c;
    cin >> food_r >> food_c;
    cin >> r >> c;

    vector <string> grid;

    for(int i=0; i<r; i++) {
        string s; cin >> s;
        grid.push_back(s);
    }

    dfs( r, c, pacman_r, pacman_c, food_r, food_c, grid);

    return 0;
}
