#include "../header.h"

class Solution {
public:
    int longestSquareStreak(vector<int> nums) {
        sort(nums.begin(), nums.end());
        unordered_map<int, int> prev2max;
        const int n = nums.size();
        for (int i = 0; i < n; ++i) {
            int prev0 = sqrt(nums[i]);

            auto t = [&](int prev) {
                if (prev*prev == nums[i]) {
                    if (prev2max.count(prev)) {
                        prev2max[nums[i]] = max(max(prev2max[prev] + 1, 1), prev2max[nums[i]]);
                    } else {
                        prev2max[nums[i]] = max(1, prev2max[nums[i]]);
                    }
                } else {
                    prev2max[nums[i]] = max(1, prev2max[nums[i]]);
                }
            };

            for (int prev = prev0 - 1; prev < prev0 + 2; ++prev) {
                t(prev);
            }
            for (int prev = -prev0 - 1; prev < -prev0 + 2; ++prev) {
                t(prev);
            }
        }
        // cerr << prev2max << endl;
        int result = -1;
        for (const auto& p: prev2max) {
            result = max(result, p.second);
        }
        return (result > 1) ? result : -1;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.longestSquareStreak({4,3,6,16,8,2}) << endl;

    return 0;
}
