class Solution {
public:
    int smallestBalancedIndex(vector<int>& nums) {
        int n = nums.size();
        if (n == 0) return -1;

        long long total_sum = 0;
        for (int x : nums) total_sum += x;

        long long left_sum = 0;
        for (int i = 0; i < n; ++i) {
            long long right_sum = total_sum - left_sum - nums[i];
            if (left_sum == right_sum) return i;
            left_sum += nums[i];
        }
        return -1;
    }
};
