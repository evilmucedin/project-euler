#include "../header.h"

#include "../tree.h"

class Solution {
   public:
    TreeNode* buildTree(const vector<int>& inorder, int inL, int inR, const vector<int>& postorder, int postL,
                        int postR, const unordered_map<int, int>& value2pos) {
        if (inL >= inR || postL >= postR) {
            return nullptr;
        }
        auto result = new TreeNode(postorder[postR - 1]);
        int mid = value2pos.find(postorder[postR - 1])->second;
        result->left = buildTree(inorder, inL, mid, postorder, postL, postL + mid - inL, value2pos);
        result->right = buildTree(inorder, mid + 1, inR, postorder, postL + mid - inL + 1, postR - 1, value2pos);
        return result;
    }

    TreeNode* buildTree(const vector<int>& inorder, const vector<int>& postorder) {
        unordered_map<int, int> value2pos;
        for (int i = 0; i < inorder.size(); ++i) {
            value2pos.emplace(inorder[i], i);
        }

        return buildTree(inorder, 0, inorder.size(), postorder, 0, postorder.size(), value2pos);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.buildTree({9, 3, 15, 20, 7}, {9, 15, 7, 20, 3}) << endl;

    return 0;
}
