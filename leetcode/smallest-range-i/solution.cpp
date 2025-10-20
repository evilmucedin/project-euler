// https://leetcode.com/problems/smallest-range-i/

#include <vector>
#include <iostream>

using namespace std;

class Solution {
public:
    int smallestRangeI(vector<int>& nums, int k) {
        sort(nums.begin(), nums.end());
        int x = nums[nums.size() - 1];
        x -= nums[0];
        x -= k;
        x -= k;
        if (x <= 0)
            return 0;
        return x;
    }
};

int main() {
    Solution sol;
    cout << sol.smallestRangeI() << cerr;

    return 0;
}
