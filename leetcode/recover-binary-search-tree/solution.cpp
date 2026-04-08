#include <iostream>
<<<<<<< HEAD
=======
#include <vector>
#include <algorithm>
>>>>>>> 5dd2c62abcc99357697d23886080f074089f813f

using namespace std;

/**
 * Definition for a binary tree node.
 */
 struct TreeNode {
     int val;
     TreeNode *left;
     TreeNode *right;
     TreeNode() : val(0), left(nullptr), right(nullptr) {}
     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 };

class Solution {
public:
<<<<<<< HEAD
    void recoverTree(TreeNode* root) {

=======
    static TreeNode* findMin(TreeNode* node) {
        if (!node)
            return node;
        auto result = node;
        auto lResult = findMin(node->left);
        if (lResult && lResult->val < result->val)
            result = lResult;
        auto rResult = findMin(node->right);
        if (rResult && rResult->val < result->val)
            result = rResult;
        return result;
    }

    static TreeNode* findMax(TreeNode* node) {
        if (!node)
            return node;
        auto result = node;
        auto lResult = findMax(node->left);
        if (lResult && lResult->val > result->val)
            result = lResult;
        auto rResult = findMax(node->right);
        if (rResult && rResult->val > result->val)
            result = rResult;
        return result;
    }

    void recoverTree_(TreeNode* root) {
        if (root) {
            auto max = findMin(root);
            if (max != root && max->val < root->val) {
                swap(root->val, max->val);
                return;
            }
            recoverTree(root->right);
            auto min = findMin(root);
            if (min != root && min->val > root->val) {
                swap(root->val, min->val);
                return;
            }
            recoverTree(root->left);
        }
    }

    void fill(vector<int>& v, const TreeNode* root) {
        if (root) {
            fill(v, root->left);
            v.push_back(root->val);
            fill(v, root->right);
        }
    }

    void unfill(const vector<int>& v, int& i, TreeNode* root) {
        if (root) {
            unfill(v, i, root->left);
            root->val = v[i];
            ++i;
            unfill(v, i, root->right);
        }
    }

    void recoverTree(TreeNode* root) {
        vector<int> v;
        fill(v, root);
        sort(v.begin(), v.end());
        int i = 0;
        unfill(v, i, root);
>>>>>>> 5dd2c62abcc99357697d23886080f074089f813f
    }
};

int main() {
    return 0;
}
