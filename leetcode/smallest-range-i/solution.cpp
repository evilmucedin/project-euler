// https://leetcode.com/problems/smallest-range-i/

#include <vector>
#include <iostream>

using namespace std;

class Solution {
public:
    int smallestRangeI(const vector<int>& nums, int k) {
        int a = nums[0];
        int b = nums[0];
        for (int i = 1; i < nums.size(); ++i) {
            const int n = nums[i];
            if (n > a)
                a = n;
            if (n < b)
                b = n;
        }
        int x = a - b;
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
