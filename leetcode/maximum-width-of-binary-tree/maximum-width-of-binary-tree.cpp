#include "../header.h"

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    vector<unsigned long long int> minD;
    vector<unsigned long long int> maxD;

    void num(TreeNode* n, unsigned long long int val, int d) {
        if (n) {
        while (d >= minD.size()) {
            minD.emplace_back(1LL << 63);
            maxD.emplace_back(0);
        }
            val = val & ((1LL << 61) - 1);

            n->val = val;
            minD[d] = std::min<unsigned long long int>(minD[d], val);
            maxD[d] = std::max<unsigned long long int>(maxD[d], val);

            num(n->left, 2*val, d + 1);
            num(n->right, 2*val + 1, d + 1);
        }
    }

    int widthOfBinaryTree(TreeNode* root) {
        num(root, 0, 0);
        long long int maxW = 0;
        for (int i = 0; i < minD.size(); ++i) {
            maxW = max<long long int>(maxW, maxD[i] - minD[i] + 1);
        }
        return maxW;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol. << endl;

    return 0;
}
