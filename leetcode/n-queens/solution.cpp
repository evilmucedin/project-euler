#include <vector>
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

class Solution {
public:
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> results;
        vector<string> board(n, string(n, '.'));
        solve(n, 0, board, results);
        return results;
    }

private:
    void solve(int n, int row, vector<string>& board, vector<vector<string>>& results) {
        if (row == n) {
            results.push_back(board);
            return;
        }

        for (int col = 0; col < n; ++col) {
            if (isSafe(n, row, col, board)) {
                board[row][col] = 'Q';
                solve(n, row + 1, board, results);
                board[row][col] = '.'; // Backtrack
            }
        }
    }

    bool isSafe(int n, int row, int col, const vector<string>& board) {
        // Check this column in previous rows
        for (int i = 0; i < row; ++i) {
            if (board[i][col] == 'Q') {
                return false;
            }
        }

        // Check upper left diagonal
        for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; --i, --j) {
            if (board[i][j] == 'Q') {
                return false;
            }
        }

        // Check upper right diagonal
        for (int i = row - 1, j = col + 1; i >= 0 && j < n; --i, --j) {
            if (board[i][j] == 'Q') {
                return false;
            }
        }

        return true;
    }
};

// Example usage in main function
int main() {
    Solution sol;
    int n = 4; // Change N for different board sizes (e.g., 8 for 8-queens)
    vector<vector<string>> solutions = sol.solveNQueens(n);

    cout << "Found " << solutions.size() << " solutions for N = " << n << ":" << endl;
    for (const auto& solution : solutions) {
        for (const auto& row : solution) {
            cout << row << endl;
        }
        cout << endl;
    }

    return 0;
}

