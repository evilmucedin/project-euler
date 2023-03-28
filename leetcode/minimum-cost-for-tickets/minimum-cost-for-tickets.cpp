#include "../header.h"

class Solution {
public:
    static constexpr int DAYS[] = {1, 7, 30};
    static constexpr int INF = 1234567890;

    int mincostTickets(const vector<int>& days_, const vector<int>& costs) {
        auto days = days_;
        // days.emplace_back(-1000);
        sort(days.begin(), days.end());
        const int n = days.size();
        vector<int> dp(n, INF);
        dp[0] = min({costs[0], costs[1], costs[2]});
        for (int i = 1; i < n; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (i > 0) {
                    dp[i] = min(dp[i], dp[i - 1] + costs[j]);
                } else {
                    dp[i] = min(dp[i], costs[j]);
                }
                int start = i;
                while (start >= 0 && days[start] + DAYS[j] > days[i]) {
                    --start;
                }
                dp[i] = min(dp[i], dp[start] + costs[j]);
            }
        }

        return dp.back();
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.mincostTickets({1, 4, 6, 7, 8, 20}, {2, 7, 15}) << endl;
    cerr << sol.mincostTickets({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 31}, {2, 7, 15}) << endl;

    return 0;
}
