#include "../header.h"

class Solution {
public:
    /*
    long long countFairPairs(vector<int>& nums, int lower, int upper) {
        sort(nums.begin(), nums.end());
        int n = nums.size();
        long long res = 0;
        int left = 0, right = 1;
        while (right < n) {
            if (nums[right] - nums[left] >= lower) {
                if (nums[right] - nums[left] <= upper) {
                    res += right - left;
                }
                right++;
            } else {
                left++;
                if (left == right) {
                    right++;
                }
            }
        }
        return res;
    }
    */

    long long countFairPairs(const vector<int>& nums2, int lower, int upper) {
        auto nums = nums2;
        sort(nums.begin(), nums.end());
        if (lower > upper) {
            return 0;
        }
        const int n = nums.size();
        int left = n - 1;
        int right = n - 1;
        long long result = 0;
        for (int i = 0; i < n; ++i) {
            while (left > 0 && nums[i] + nums[left - 1] >= lower) {
                --left;
            }
            while (left < n && nums[i] + nums[left] < lower) {
                ++left;
            }
            while (right + 1 < n && nums[i] + nums[right + 1] <= upper) {
                ++right;
            }
            while (right >= left && nums[i] + nums[right] > upper) {
                --right;
            }
            cerr << i << " " << nums[i] << " " << left << " " << right << endl;
            if (right < left) {
                continue;
            }
            int iright = max(i + 1, right);
            int ileft = max(i + 1, left);
            if (iright >= ileft) {
                if (nums[i] + nums[ileft] >= lower && nums[i] + nums[ileft] <= upper) {
                    if (nums[i] + nums[iright] >= lower && nums[i] + nums[iright] <= upper) {
                        result += iright - ileft + 1;
                    }
                }
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.countFairPairs({0,1,7,4,4,5}, 11, 11) << endl;
    cerr << sol.countFairPairs({1, 7, 9, 2, 5}, 11, 11) << endl;

    return 0;
}
