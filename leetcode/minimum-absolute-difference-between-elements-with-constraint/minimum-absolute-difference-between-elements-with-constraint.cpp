#include "../header.h"

#include <set>
#include <climits>

class Solution {
   public:
    int minAbsoluteDifference(const vector<int>& nums0, int x) {
        auto nums = nums0;

        int n = nums.size();

        if (x == 0) {
            return 0;

            sort(nums.begin(), nums.end());

            int ans = nums[1] - nums[0];
            for (int i = 1; i < n; ++i) {
                ans = min(ans, nums[i] - nums[i - 1]);
            }

            return ans;
        }

        if (n < 10000) {
            int ans = INT_MAX;
            for (int i = 0; i < n; ++i) {
                for (int j = i + x; j < n; ++j) {
                    ans = min(ans, abs(nums[i] - nums[j]));
                }
            }
            return ans;
        }


        multiset<int> far;
        int ans = INT_MAX;

        for (int i = x + 1; i < n; ++i) {
            far.emplace(nums[i]);
        }

        for (int i = 0; i < n; ++i) {
            if (i + x - 1 < n) {
                far.erase(nums[i + x - 1]);
            }
            if (i - x >= 0) {
                far.insert(nums[i - x]);
            }

            cerr << nums[i] << " " << far << endl;

            // find the closest element to the current one in the multiset
            auto it = far.lower_bound(nums[i]);
            if (it != far.end()) {
                // update the answer with the minimum of the current and previous differences
                ans = min(ans, abs(nums[i] - *it));
                if (it != far.begin()) {
                    // check the previous element in the multiset as well
                    it--;
                    ans = min(ans, abs(nums[i] - *it));
                }
            }
        }

        return ans;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minAbsoluteDifference({4, 3, 2, 4}, 2) << endl;
    cerr << sol.minAbsoluteDifference({5, 3, 2, 10, 15}, 1) << endl;

    return 0;
}
