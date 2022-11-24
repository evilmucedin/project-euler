#include "../header.h"

#include <set>

class Solution {
   public:
    struct Key {
        int x;
        int y;
        string s;

        bool operator<(const Key& rhs) const {
            if (x != rhs.x) {
                return x < rhs.x;
            }
            if (y != rhs.y) {
                return y < rhs.y;
            }
            return s < rhs.s;
        }
    };
    set<Key> visited_;

    void dfs(vector<vector<char>>& board, int x, int y, string now, const unordered_set<string>& prefixes,
             const unordered_set<string>& words, vector<string>& result) {
        if (x < 0 || x >= board.size()) {
            return;
        }
        if (y < 0 || y >= board[x].size()) {
            return;
        }
        if (board[x][y] == '#') {
            return;
        }

        Key key{x, y, now};
        if (visited_.count(key)) {
            return;
        }
        visited_.emplace(key);

        // cerr << x << " " << y << " " << now << endl;
:q

        now += board[x][y];
        if (!prefixes.count(now)) {
            return;
        }
        if (words.count(now)) {
            result.emplace_back(now);
        }
        char prev = board[x][y];
        board[x][y] = '#';
        // static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1, 1, -1, -1, 1, 1, 1, -1, -1};
        static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1};
        for (int i = 0; i < 4; ++i) {
            dfs(board, x + DIRS[2 * i], y + DIRS[2 * i + 1], now, prefixes, words, result);
        }
        board[x][y] = prev;
    }

    vector<string> findWords(const vector<vector<char>>& board, const vector<string>& words) {
        unordered_set<string> prefixes;
        for (const auto& s : words) {
            for (int i = 0; i <= s.size(); ++i) {
                prefixes.emplace(s.substr(0, i));
            }
        }
        unordered_set<string> sWords(words.begin(), words.end());
        vector<string> result;
        for (int i = 0; i < board.size(); ++i) {
            for (int j = 0; j < board[i].size(); ++j) {
                visited_.clear();
                auto board2 = board;
                dfs(board2, i, j, "", prefixes, sWords, result);
            }
        }
        sort(result.begin(), result.end());
        result.erase(unique(result.begin(), result.end()), result.end());
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findWords({{'a', 'b', 'c', 'e'}, {'z', 'z', 'd', 'z'}, {'z', 'z', 'c', 'z'}, {'z', 'a', 'b', 'z'}},
                          {"abcdce"})
         << endl;

    return 0;
}
