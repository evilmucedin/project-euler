#include "../header.h"

class Solution {
public:
    int maxProduct(const vector<int>& nums) {
        long long result = 0;
        doMax<long long>(result, *max_element(nums.begin(), nums.end()));
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxProduct({2, 3, -2, 4}) << endl;
    cerr << sol.maxProduct({-2, 0, -1}) << endl;

    return 0;
}
