#include "../header.h"

#include <lib/unionFind.h>

class Solution {
public:
    int longestConsecutive(const vector<int>& nums) {
        UnionFind<int> uf;

        for (int i : nums) {
            if (!uf.has(i)) {
                uf.add(i);
                uf.unite(i, i + 1);
                uf.unite(i, i - 1);
            }
        }

        int res = 0;
        for (const auto& s: uf.components()) {
            if (s.size > res) {
                res = s.size;
           }
        }

        return res;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.longestConsecutive({0, 3, 7, 2, 5, 8, 4, 6, 0, 1}) << endl;

    return 0;
}
