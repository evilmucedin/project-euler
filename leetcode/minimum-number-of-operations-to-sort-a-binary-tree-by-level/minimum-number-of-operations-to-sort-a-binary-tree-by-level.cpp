#include "../header.h"
#include "../tree.h"

class Solution {
   public:
    static int minSwaps(const vector<int>& arr) {
        const int n = arr.size();
        vector<pair<int, int>> arrPos(n);
        for (int i = 0; i < n; i++) {
            arrPos[i].first = arr[i];
            arrPos[i].second = i;
        }

        sort(arrPos.begin(), arrPos.end());

        vector<bool> vis(n, false);

        int ans = 0;

        for (int i = 0; i < n; i++) {
            if (vis[i] || arrPos[i].second == i) {
                continue;
            }

            int cycleSize = 0;
            int j = i;
            while (!vis[j]) {
                vis[j] = 1;

                j = arrPos[j].second;
                ++cycleSize;
            }

            if (cycleSize > 0) {
                ans += cycleSize - 1;
            }
        }

        return ans;
    }

    static void dfs(TreeNode* root, vector<vector<int>>& levels, int level) {
        if (!root) {
            return;
        }
        while (level >= levels.size()) {
            levels.emplace_back();
        }
        levels[level].emplace_back(root->val);
        dfs(root->left, levels, level + 1);
        dfs(root->right, levels, level + 1);
    }

    int minimumOperations(TreeNode* root) {
        vector<vector<int>> levels;
        dfs(root, levels, 0);
        int result = 0;
        for (const auto& l : levels) {
            result += minSwaps(l);
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minimumOperations(stringToTreeNode("[1,4,3,7,6,8,5,null,null,null,null,9,null,10]")) << endl;

    return 0;
}
