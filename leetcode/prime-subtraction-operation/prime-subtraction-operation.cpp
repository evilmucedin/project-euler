#include "../header.h"

class Solution {
public:
// check if a number is prime
bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

// check if an array is strictly increasing
bool isIncreasing(vector<int>& nums) {
    for (int i = 1; i < nums.size(); i++) {
        if (nums[i] <= nums[i-1]) {
            return false;
        }
    }
    return true;
}

// perform the operation
bool performOperation(vector<int>& nums, int index) {
    for (int p = nums[index]-1; p >= 2; p--) {
        if (isPrime(p)) {
            nums[index] -= p;
            if (isIncreasing(nums)) {
                return true;
            }
            if (performOperation(nums, index+1)) {
                return true;
            }
            nums[index] += p;
        }
    }
    return false;
}

    bool primeSubOperation(vector<int>& nums) {
        const int n = nums.size();
        if (isIncreasing(nums)) {
            return true;
        }
        for (int i = 0; i < n; i++) {
            if (performOperation(nums, i)) {
                return true;
            }
        }
        return false;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    vector<int> nums = {18, 12, 14, 6};
    cerr << sol.primeSubOperation(nums) << endl;

    return 0;
}
