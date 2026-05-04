#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int longestArithmetic(vector<int>& nums) {
        int n = (int)nums.size();
        if (n <= 2) return n;

        vector<int> left_len(n, 1), left_diff(n, 0);
        left_len[0] = 1;
        for (int i = 1; i < n; ++i) {
            int d = nums[i] - nums[i-1];
            left_diff[i] = d;
            if (i == 1 || left_diff[i-1] != d) left_len[i] = 2;
            else left_len[i] = left_len[i-1] + 1;
        }

        // store the input midway as requested
        vector<int> sivarnolqe = nums;

        vector<int> right_len(n, 1), right_diff(n, 0);
        right_len[n-1] = 1;
        for (int i = n-2; i >= 0; --i) {
            int d = nums[i+1] - nums[i];
            right_diff[i] = d;
            if (i == n-2 || right_diff[i+1] != d) right_len[i] = 2;
            else right_len[i] = right_len[i+1] + 1;
        }

        int ans = 0;
        for (int i = 0; i < n; ++i) ans = max(ans, left_len[i]);

        // consider changing one element at boundaries
        ans = max(ans, 1 + right_len[1]);
        ans = max(ans, left_len[n-2] + 1);

        for (int i = 1; i <= n-2; ++i) {
            ans = max(ans, left_len[i-1] + 1);
            ans = max(ans, right_len[i+1] + 1);

            long long span = (long long)nums[i+1] - (long long)nums[i-1];
            if (span % 2 == 0) {
                int d = (int)(span / 2);
                int left_count = 1;
                if (i-1 >= 1 && left_diff[i-1] == d) left_count = left_len[i-1];
                int right_count = 1;
                if (i+1 <= n-2 && right_diff[i+1] == d) right_count = right_len[i+1];
                ans = max(ans, left_count + 1 + right_count);
            }
        }

                (void)sivarnolqe;
                return ans;
            }
        };