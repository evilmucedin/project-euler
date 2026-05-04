#include "../header.h"

class Solution {
public:
    int change(int amount, const vector<int>& coins) {
        vector<int> dp(amount + 1);
        dp[0] = 1;
        for (int i = 0; i <= amount; ++i) {
            if (!dp[i]) {
                continue;
            }

            for (int c: coins) {
                int s = c + i;
                if (s <= amount) {
                    dp[s] += dp[i];
                }
            }
        }

        cerr << dp << endl;

        return dp[amount];
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.change(5, {1, 2, 5}) << endl;

    return 0;
}
