#include "../header.h"

#include <lib/unionFindDense.h>

class Solution {
public:
    static bool similar(const string& a, const string& b) {
        if (a.size() != b.size()) {
            return false;
        }

        int count = 0;
        for (int i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) {
                ++count;
            }
        }

        return count == 0 || count == 2;
    }

    int numSimilarGroups(const vector<string>& strs) {
        const int n = strs.size();
        UnionFindDense uf(n);
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (similar(strs[i], strs[j])) {
                    uf.unite(i, j);
                }
            }
        }
        return uf.components().size();
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.numSimilarGroups({"tars", "rats", "arts", "star"}) << endl;
    cerr << sol.numSimilarGroups({"omv", "ovm"}) << endl;

    return 0;
}
