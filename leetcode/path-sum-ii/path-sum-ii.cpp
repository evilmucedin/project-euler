#include "../header.h"
#include "../tree.h"

class Solution {
   public:
    static void pathSum_(TreeNode* root, int targetSum, vector<vector<int>>& result, vector<int>& now) {
        if (root == nullptr) {
            return;
        }
        now.emplace_back(root->val);
        targetSum -= root->val;
        if (targetSum == 0 && !now.empty() && !root->left && !root->right) {
            result.emplace_back(now);
        }
        pathSum_(root->left, targetSum, result, now);
        pathSum_(root->right, targetSum, result, now);
        now.pop_back();
    }

    vector<vector<int>> pathSum(TreeNode* root, int targetSum) {
        vector<vector<int>> result;
        vector<int> now;
        pathSum_(root, targetSum, result, now);
        // result.erase(unique(result.begin(), result.end()), result.end());
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.pathSum(makeTree({1}), 1) << endl;

    return 0;
}
