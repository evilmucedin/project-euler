#include "../header.h"

#include "lib/unionFindDense.h"

class Solution {
public:
    bool canTraverseAllPairs(const vector<int>& nums) {
        const int n = nums.size();
        UnionFindDense uf(n);
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (__gcd(nums[i], nums[j]) > 1) {
                    uf.unite(i, j);
                }
            }
        }
        return uf.components().size() == 1;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.canTraverseAllPairs({2, 3, 6}) << endl;
    cerr << sol.canTraverseAllPairs({3, 9, 5}) << endl;
    cerr << sol.canTraverseAllPairs({4, 3, 12, 8}) << endl;

    return 0;
}
