#include "../header.h"

#include <set>

#include <lib/trie.h>

class Solution {
   public:
    struct Key {
        int x;
        int y;
        string s;
        int h;

        bool operator<(const Key& rhs) const {
            if (x != rhs.x) {
                return x < rhs.x;
            }
            if (y != rhs.y) {
                return y < rhs.y;
            }
            if (h != rhs.h) {
                return h < rhs.h;
            }
            return s < rhs.s;
        }
    };
    set<Key> visited_;

    using PTrieNode = Trie<char>::PTrieNode;

    void dfs(vector<vector<char>>& board, int x, int y, string now, PTrieNode nowNode, unordered_set<string>& result, int h) {
        if (x < 0 || x >= board.size()) {
            return;
        }
        if (y < 0 || y >= board[x].size()) {
            return;
        }
        if (board[x][y] == '#') {
            return;
        }


        Key key{x, y, now, h};
        if (visited_.count(key)) {
            return;
        }
        visited_.emplace(key);
        /*
        */

        if (nowNode == nullptr) {
            return;
        }

        now += board[x][y];
        nowNode = nowNode->getNodeConst(board[x][y]);
        if (nowNode == nullptr) {
            return;
        }
        if (nowNode->isWord()) {
            result.emplace(now);
        }
        char prev = board[x][y];
        board[x][y] = '#';
        h += x << 5;
        h += y << 7;
        // static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1, 1, -1, -1, 1, 1, 1, -1, -1};
        static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1};
        for (int i = 0; i < 4; ++i) {
            dfs(board, x + DIRS[2 * i], y + DIRS[2 * i + 1], now, nowNode, result, h);
        }
        board[x][y] = prev;
    }

    vector<string> findWords(const vector<vector<char>>& board0, const vector<string>& words) {
        auto board = board0;
        vector<int> counts(26);
        for (int i = 0; i < board.size(); ++i) {
            for (int j = 0; j < board[i].size(); ++j) {
                ++counts[board[i][j] - 'a'];
            }
        }

        Trie<char> t('a', 'z');
        for (const auto& s : words) {
            vector<int> counts2(26);
            bool good = true;
            for (char ch: s) {
                ++counts2[ch - 'a'];
                if (counts2[ch - 'a'] > counts[ch - 'a']) {
                    good = false;
                }
            }

            if (good) {
                t.addWord(s);
            }
        }

        unordered_set<string> result;
        for (int i = 0; i < board.size(); ++i) {
            for (int j = 0; j < board[i].size(); ++j) {
        // visited_.clear();
                // auto board2 = board;
                dfs(board, i, j, "", t.root(), result, 0);
            }
        }
        return {result.begin(), result.end()};
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findWords(
                {vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                 vector<char>{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'}},
                {"a", "aa", "aaa", "aaaa", "aaaaa", "aaaaaa", "aaaaaaa", "aaaaaaaa", "aaaaaaaaa", "aaaaaaaaaa"}

                )
         << endl;

    return 0;
}
