#include "../header.h"
#include "../tree.h"

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
    void makeG(const TreeNode* root, const TreeNode* parent,
               unordered_map<const TreeNode*, vector<const TreeNode*>>& g) {
        if (g.count(root) == 0) {
            if (parent) {
                g[root].emplace_back(parent);
            }
            if (root->left) {
                g[root].emplace_back(root->left);
                makeG(root->left, root, g);
            }
            if (root->right) {
                g[root].emplace_back(root->right);
                makeG(root->right, root, g);
            }
        }
    }

    const TreeNode* findN(const TreeNode* root, int val) {
        if (root) {
            if (root->val == val) {
                return root;
            }
            auto l = findN(root->left, val);
            if (l) {
                return l;
            }
            auto r = findN(root->right, val);
            if (r) {
                return r;
            }
        }
        return nullptr;
    }

    int maxD(const TreeNode* root, unordered_map<const TreeNode*, vector<const TreeNode*>>& g,
             unordered_map<const TreeNode*, int>& d, unordered_set<const TreeNode*>& visited) {
        if (!root) {
            return 0;
        }
        if (visited.count(root) == 0) {
            visited.emplace(root);

            if (d[root] != 0) {
                return d[root];
            }
            int mx = 1;
            for (const auto& v : g[root]) {
                mx = max(mx, maxD(v, g, d, visited) + 1);
            }
            return d[root] = mx;
        }
    }

    int amountOfTime_(TreeNode* root, int t) {
        if (!root) return 0;

        queue<TreeNode*> q;
        q.push(root);
        int time = 0;

        while (!q.empty()) {
            int n = q.size();
            bool infected = false;

            for (int i = 0; i < n; ++i) {
                TreeNode* current = q.front();
                q.pop();

                if (current->val == t) {
                    infected = true;
                    break;
                }

                if (current->left) q.push(current->left);
                if (current->right) q.push(current->right);
            }

            if (infected) return time;
            ++time;
        }

        return -1;  // Return -1 if the target node was not found
    }

    int amountOfTime(TreeNode* root, int start) {
        unordered_map<const TreeNode*, vector<const TreeNode*>> g;
        makeG(root, nullptr, g);
        auto startN = findN(root, start);
        unordered_map<const TreeNode*, int> d;
        unordered_set<const TreeNode*> visited;
        return maxD(startN, g, d, visited);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.amountOfTime(stringToTreeNode("[1,5,3,null,4,10,6,9,2]"), 3) << endl;

    return 0;
}
