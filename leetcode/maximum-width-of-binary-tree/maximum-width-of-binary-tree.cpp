#include "../header.h"
#include "../tree.h"

#include <lib/bigint.h>

class Solution {
   public:
    using T = BigInteger;
    vector<bool> hasMinD;
    vector<T> minD;
    vector<T> maxD;

    void num(TreeNode* n, const T& val, int d, bool debug) {
        if (n) {
            if (debug) {
                cerr << n->val << " d: " << d << " val: " << val.toU32() << endl;
            }

            while (d >= minD.size()) {
                hasMinD.emplace_back(false);
                minD.emplace_back(0);
                maxD.emplace_back(0);
            }

            if (!hasMinD[d]) {
                minD[d] = val;
                hasMinD[d] = true;
            } else {
                minD[d] = std::min<T>(minD[d], val);
            }
            maxD[d] = std::max<T>(maxD[d], val);

            num(n->left, val * 2, d + 1, debug);
            num(n->right, val * 2 + 1, d + 1, debug);
        }
    }

    int widthOfBinaryTree(TreeNode* root, bool debug = false) {
        hasMinD.clear();
        minD.clear();
        maxD.clear();
        num(root, 0, 0, debug);
        T maxW = 0;
        for (int i = 0; i < minD.size(); ++i) {
            maxW = max<T>(maxW, maxD[i] - minD[i] + 1);
        }
        // cerr << minD << endl << maxD << endl;
        return maxW;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.widthOfBinaryTree(makeTree({1, 3, 2, 5, null, null, 9, 6, null, 7})) << endl;
    // printTree(makeTree({1, 3, 2, 5, 3, null, 9}));
    cerr << sol.widthOfBinaryTree(makeTree({1, 3, 2, 5, 3, null, 9})) << endl;
    cerr << sol.widthOfBinaryTree(
                makeTree({0, 0, 0,    null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null, 0, 0, null, null,
                          0, 0, null, null, 0, 0, null, null, 0, 0, null}))
         << endl;
    cerr << sol.widthOfBinaryTree(makeTree({1, 1, 1, 1, 1, 1, 1, null, null, null, 1,    null, null, null, null,
                                            2, 2, 2, 2, 2, 2, 2, null, 2,    null, null, 2,    null, 2}))
         << endl;

    /*
    prettyPrintTree(makeTree({1, 1, 1, 1, 1, 1, 1, null, null, null, 1,    null, null, null, null,
                              2, 2, 2, 2, 2, 2, 2, null, 2,    null, null, 2,    null, 2}));
    */

    cerr << sol.widthOfBinaryTree(makeTree({
        1, 1, 1, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1,
            null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1,
            null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1,
            null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1,
            null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, 1, null, null, 1, 1, null, 1, null, 1,
            null, 1, null, 1, null})) << endl;

    return 0;
}
