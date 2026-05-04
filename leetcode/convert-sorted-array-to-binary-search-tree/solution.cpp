#include <vector>
#include <iostream>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    TreeNode* sortedArrayToBST(const std::vector<int>& nums) {
        return buildBST(nums, 0, nums.size() - 1);
    }

private:
    TreeNode* buildBST(const std::vector<int>& nums, int start, int end) {
        // Base case: if the subarray is empty, return nullptr
        if (start > end) {
            return nullptr;
        }

        // Find the middle element to be the root
        int mid = start + (end - start) / 2;
        TreeNode* root = new TreeNode(nums[mid]);

        // Recursively build the left and right subtrees
        root->left = buildBST(nums, start, mid - 1);
        root->right = buildBST(nums, mid + 1, end);

        return root;
    }
};

// Helper function to print the tree (in-order traversal for verification)
void inOrderTraversal(TreeNode* root) {
    if (root == nullptr) {
        return;
    }
    inOrderTraversal(root->left);
    std::cout << root->val << " ";
    inOrderTraversal(root->right);
}

int main() {
    std::vector<int> nums = {-10, -3, 0, 5, 9};
    Solution sol;
    TreeNode* root = sol.sortedArrayToBST(nums);

    std::cout << "In-order traversal of the constructed BST: ";
    inOrderTraversal(root);
    std::cout << std::endl;

    return 0;
}
