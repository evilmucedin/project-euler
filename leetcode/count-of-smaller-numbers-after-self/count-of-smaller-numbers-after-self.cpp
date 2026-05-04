#include "../header.h"

#include <lib/segmentTree.h>

class Solution {
public:
    static constexpr int OFFSET = 20000;

    vector<int> countSmaller(const vector<int>& nums) {
        vector<int> result;

        SegmentTree<int, Sum<int>> st(2*OFFSET, 0, Sum<int>());
        for (int i = nums.size() - 1; i >= 0; --i) {
            int index = nums[i] + OFFSET;
            result.emplace_back(st.getAggr(0, index));
            st.set(index, 1);
        }

        reverse(result.begin(), result.end());
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.countSmaller({5, 2, 6, 1}) << endl;

    return 0;
}
