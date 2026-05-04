#include "../header.h"
#include "lib/graph/pushRelabel.h"

class Solution {
   public:
    int maxStudents(const vector<vector<char>>& seats) {
        static const vector<vector<int>> DIR = {{0, -1}, {-1, -1}, {1, -1}, {0, 1}, {-1, 1}, {1, 1}};
        int m = seats.size();
        int n = seats[0].size();
        int s = m * n;
        int t = m * n + 1;
        PushRelabel pr(m * n + 2);
        int seatcnt = 0;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (seats[i][j] == '#') {
                    continue;
                }
                ++seatcnt;
                if (j % 2 == 0) {
                    pr.addEdge(s, i * n + j, 1);
                    for (int d = 0; d < 6; ++d) {
                        int ni = i + DIR[d][0];
                        int nj = j + DIR[d][1];
                        if (ni < 0 || ni >= m || nj < 0 || nj >= n || seats[ni][nj] == '#') {
                            continue;
                        }
                        pr.addEdge(i * n + j, ni * n + nj, 1);
                    }
                } else {
                    pr.addEdge(i * n + j, t, 1);
                }
            }
        }
        auto flow = pr.calc(s, t);
        return seatcnt - flow;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxStudents(
                {{'#', '.', '#', '#', '.', '#'}, {'.', '#', '#', '#', '#', '.'}, {'#', '.', '#', '#', '.', '#'}})
         << endl;

    return 0;
}
