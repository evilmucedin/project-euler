#include "../header.h"

class Solution {
public:
    int firstMissingPositive(vector<int>& nums) {
        for (int i = 0; i < nums.size(); ++i) {
            int put = nums[i];
            while (put >= 1 && put <= nums.size()) {
                if (put != nums[put - 1]) {
                    swap(put, nums[put - 1]);
                } else {
                    break;
                }
            }
        }
        for (int i = 0; i < nums.size(); ++i) {
            if (nums[i] != i + 1) {
                return i + 1;
            }
        }
        return nums.size() + 1;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    vector<int> a1 = {1,2,0};
    cerr << sol.firstMissingPositive(a1) << endl;
    vector<int> a2 = {3,4,-1,1};
    cerr << sol.firstMissingPositive(a2) << endl;
    vector<int> a3 = {7,8,9,11,12};
    cerr << sol.firstMissingPositive(a3) << endl;

    return 0;
}
