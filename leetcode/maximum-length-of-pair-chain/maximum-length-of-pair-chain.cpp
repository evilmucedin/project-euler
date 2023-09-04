#include "../header.h"

class Solution {
public:
    int findLongestChain(const vector<vector<int>>& pairs) {
        const int n = pairs.size();
        vector<int> mxl(n + 1);
        mxl[0] = 0;
        int mx = 0;
        for (int i = 1; i <= n; ++i) {
            for (int j = 0; j < i; ++j) {
                if (j == 0 || pairs[j - 1].back() < pairs[i - 1].front()) {
                    mxl[i] = max(mxl[i], mxl[j] + 1);
                }
            }
            mx = max(mx, mxl[i]);
        }
        return mx;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findLongestChain({{1, 2}, {2, 3}, {3, 4}}) << endl;

    return 0;
}
