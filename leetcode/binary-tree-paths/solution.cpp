#include <vector>
#include <string>
#include <iostream>

using namespace std;

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

public:
    void binaryTreePathsS(TreeNode* root, const string& s, vector<string>& result) {
	    if (!root) {
		    result.push_back(s);
		    return;
	    }
	    string ss = s + to_string(root->val);
	    if (root->left || root->right) {
	    	ss += "->";
		if (root->left) {
			binaryTreePathsS(root->left, ss, result);
		}
		if (root->right) {
			binaryTreePathsS(root->right, ss, result);
		}
	    } else {
		    result.push_back(ss);
	    }
    }

    vector<string> binaryTreePaths(TreeNode* root) {
    	vector<string> result;
	string ss;
	binaryTreePathsS(root, ss, result);
	return result;	
    }
};

string toString(const vector<string>& ss) {
	string result = "(";
	for (const string& s: ss) {
		result += s;
		result += ", ";
	}
	result += ")";
	return result;
}

int main() {
	Solution sol;
    	std::vector<int> nums = {-10, -3, 0, 5, 9};
    	TreeNode* root = sol.sortedArrayToBST(nums);
	cerr << toString(sol.binaryTreePaths(root)) << endl;
	return 0;
}
