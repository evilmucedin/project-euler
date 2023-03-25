#include "../header.h"

// Definition for a QuadTree node.
class Node {
public:
    bool val;
    bool isLeaf;
    Node* topLeft;
    Node* topRight;
    Node* bottomLeft;
    Node* bottomRight;

    Node() {
        val = false;
        isLeaf = false;
        topLeft = NULL;
        topRight = NULL;
        bottomLeft = NULL;
        bottomRight = NULL;
    }

    Node(bool _val, bool _isLeaf) {
        val = _val;
        isLeaf = _isLeaf;
        topLeft = NULL;
        topRight = NULL;
        bottomLeft = NULL;
        bottomRight = NULL;
    }

    Node(bool _val, bool _isLeaf, Node* _topLeft, Node* _topRight, Node* _bottomLeft, Node* _bottomRight) {
        val = _val;
        isLeaf = _isLeaf;
        topLeft = _topLeft;
        topRight = _topRight;
        bottomLeft = _bottomLeft;
        bottomRight = _bottomRight;
    }
};

class Solution {
public:
    Node* constructNode(const vector<vector<int>>& grid, int sx, int sy, int fx, int fy) {
        assert(sx < fx);
        assert(sy < fy);
        Node* result = new Node();
        result->isLeaf = (fx == sx + 1) && (fy == sy + 1);
        result->val = grid[sx][sy];
        if (!result->isLeaf) {
            int mx = (sx + fx) / 2;
            int my = (sy + fy) / 2;
            result->topLeft = constructNode(grid, sx, sy, mx, my);
            result->topRight = constructNode(grid, mx, sy, fx, my);
            result->bottomLeft = constructNode(grid, sx, my, mx, fy);
            result->bottomRight = constructNode(grid, mx, my, fx, fy);
        }
        return result;
    }

    Node* construct(const vector<vector<int>>& grid) {
        return constructNode(grid, 0, 0, grid.size(), grid.size());
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.construct({{0, 1}, {1, 0}}) << endl;

    return 0;
}
