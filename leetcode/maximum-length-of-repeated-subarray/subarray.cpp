#include "../header.h"

class Solution {
   public:
    int findLength(const vector<int>& nums1, const vector<int>& nums2) {
        vector<vector<int>> lcSuff(nums1.size() + 1, vector<int>(nums2.size() + 1));
        int result = 0;
        for (int i = 0; i <= nums1.size(); ++i) {
            for (int j = 0; j <= nums2.size(); ++j) {
                if (i == 0 || j == 0) {
                    lcSuff[i][j] = 0;
                } else if (nums1[i - 1] == nums2[j - 1]) {
                    lcSuff[i][j] = lcSuff[i - 1][j - 1] + 1;
                    result = max(result, lcSuff[i][j]);
                } else {
                    lcSuff[i][j] = 0;
                }
            }
        }

        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findLength(vector<int>{1, 2, 3, 2, 1}, vector<int>{3, 2, 1, 4, 7}) << endl;

    return 0;
}
