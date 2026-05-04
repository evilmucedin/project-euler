#include "../header.h"

class Solution {
public:
    int snakesAndLadders(const vector<vector<int>>& board) {
        const int n = board.size();
        vector<int> line;
        line.emplace_back(-1);
        for (int i = n - 1; i >= 0; --i) {
            if (i & 1) {
                for (int j = 0; j < n; ++j) {
                    line.emplace_back(board[i][j]);
                }
            } else {
                for (int j = n - 1; j >= 0; --j) {
                    line.emplace_back(board[i][j]);
                }
            }
        }

        cerr << line << endl;

        static const int INF = 10000000;
        vector<int> dp(n*n + 1, INF);
        dp[0] = 0;
        dp[1] = 0;

        for (int i = 1; i <= n*n; ++i) {
            if (line[i] == -1) {
                for (int j = i + 1; j <= min(i + 6, n*n); ++j) {
                    dp[j] = min(dp[j], dp[i] + 1);
                }
            } else {
                dp[line[i]] = min(dp[line[i]], dp[i]);
            }
        }

        return dp[n*n];
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.snakesAndLadders({{-1, -1, -1, -1, -1, -1},
                                  {-1, -1, -1, -1, -1, -1},
                                  {-1, -1, -1, -1, -1, -1},
                                  {-1, 35, -1, -1, 13, -1},
                                  {-1, -1, -1, -1, -1, -1},
                                  {-1, 15, -1, -1, -1, -1}})
         << endl;

    return 0;
}
