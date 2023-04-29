#include "../header.h"

class Solution {
   public:
    static constexpr int DAYS[] = {1, 7, 30};
    static constexpr int INF = 1234567890;

    int mincostTickets(const vector<int>& days, const vector<int>& costs) {
        const int n = days.size();
        vector<int> dp(n, INF);
        dp[0] = min({costs[0], costs[1], costs[2]});
        for (int l = 0; l < 1; ++l) {
            for (int i = 1; i < n; ++i) {
                for (int j = 0; j < 3; ++j) {
                    if (i > 0) {
                        dp[i] = min(dp[i], dp[i - 1] + costs[j]);
                    }
                    int start = i - 1;
                    while (start > 0 && days[start - 1] + DAYS[j] > days[i]) {
                        --start;
                    }
                    // cerr << OUT(i) << " " << OUT(j) << " " << OUT(start) << OUT(days[i]) << OUT(days[start]) << endl;
                    if (start >= 0 && days[start] + DAYS[j] > days[i]) {
                        if (start >= 1) {
                            dp[i] = min(dp[i], dp[start - 1] + costs[j]);
                        } else {
                            dp[i] = min(dp[i], costs[j]);
                        }
                        // cerr << "Improve: " << OUT(dp[i]) << endl;
                    }
                }
            }
        }

        // cerr << dp << endl;

        return dp.back();
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.mincostTickets({1, 4, 6, 7, 8, 20}, {2, 7, 15}) << endl;
    cerr << sol.mincostTickets({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 31}, {2, 7, 15}) << endl;
    // 50
    cerr << sol.mincostTickets({1, 2, 3, 4, 6, 8, 9, 10, 13, 14, 16, 17, 19, 21, 24, 26, 27, 28, 29}, {3, 14, 50})
         << endl;

    return 0;
}
