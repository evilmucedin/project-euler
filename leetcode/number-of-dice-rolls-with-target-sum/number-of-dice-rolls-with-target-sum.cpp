#include "../header.h"

class Solution {
public:
    int numRollsToTarget(int n, int k, int target) {
        if (target > n*k) {
            return 0;
        }
        const int MOD = 1000000007;
        const int max = n*k + 1;
        vector<int> ways(max);
        ways[0] = 1;
        for (int i = 0; i < n; ++i) {
            vector<int> newways(max);
            for (int j = 1; j <= k; ++j) {
                for (int l = 0; l < max; ++l) {
                    if (l + j < max) {
                        newways[l + j] = (newways[l + j] + ways[l]) % MOD;
                    }
                }
            }
            ways.swap(newways);
        }
        return ways[target];
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.numRollsToTarget(30, 30, 500) << endl;

    return 0;
}
