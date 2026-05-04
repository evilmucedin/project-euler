#include "../header.h"

class Solution {
public:
    bool dfs(vector<vector<char>>& board, int x, int y, const string& word, string& temp) {
        if (temp == word) {
            return true;
        }
        if (x < 0 || x >= board.size()) {
            return false;
        }
        if (y < 0 || y >= board[0].size()) {
            return false;
        }
        const  char old = board[x][y];
        if (old == '#') {
            return false;
        }
        temp += board[x][y];
        board[x][y] = '#';
        if (temp == word) {
            board[x][y] = old;
            return true;
        }
        static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1};
        for (int i = 0; i < 4; ++i) {
            int xx = x + DIRS[2 * i];
            int yy = y + DIRS[2 * i + 1];
            if (dfs(board, xx, yy, word, temp)) {
                board[x][y] = old;
                return true;
            }
        }
        temp.pop_back();
        board[x][y] = old;
        return false;
    }

    bool exist(const vector<vector<char>>& cboard, string word) {
        auto board = cboard;
        int n = board.size();
        if (n == 0) {
            return word.empty();
        }
        int m = board[0].size();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                string temp;
                if (dfs(board, i, j, word, temp)) {
                    return true;
                }
            }
        }
        return false;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.exist({{'A', 'B', 'C', 'E'}, {'S', 'F', 'C', 'S'}, {'A', 'D', 'E', 'E'}}, "ABCCED") << endl;

    return 0;
}
