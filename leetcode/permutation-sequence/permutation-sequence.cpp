#include "../header.h"

#include <algorithm>

class Solution {
public:
    string getPermutation(int n, int k) {
        vector<int> p(n);
        for (int i = 0; i < n; ++i) {
            p[i] = i + 1;
        }
        for (int i = 1; i < k; ++i) {
            next_permutation(p.begin(), p.end());
        }
        string result;
        for (int i = 0; i < n; ++i) {
            result += to_string(p[i]);
        }
        return result;
    }
};

int main() {

    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.getPermutation(4, 9) << endl;

    return 0;
}
