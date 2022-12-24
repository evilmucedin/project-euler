#include "../header.h"

class Solution {
   public:
    // 0 - no stock
    // 1 - has stock
    // 2 - stock sold today

    int maxProfit(const vector<int>& prices) {
        const int n = prices.size();
        vector<vector<int>> dp(n, vector<int>(3));
        dp[0][0] = 0;
        dp[0][1] = -prices[0];
        dp[0][2] = -1000000;
        for (int i = 1; i < n; ++i) {
            dp[i][0] = max(dp[i - 1][2], dp[i - 1][0]);
            dp[i][1] = max(dp[i - 1][0] - prices[i], dp[i - 1][1]);
            dp[i][2] = dp[i - 1][1] + prices[i];
        }
        cerr << dp << endl;
        return max({dp.back()[0], dp.back()[2]});
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxProfit({1, 2, 3, 0, 2}) << endl;

    return 0;
}
