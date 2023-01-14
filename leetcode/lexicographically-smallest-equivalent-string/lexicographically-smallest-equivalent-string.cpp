#include "../header.h"

#include "lib/unionFindDense.h"

class Solution {
public:
    char dfs(const vector<vector<int>>& g, vector<bool>& visited, char ch) {
        char result = ch;
        if (!visited[ch]) {
            visited[ch] = true;
            for (int u: g[ch]) {
                result = min(result, dfs(g, visited, u));
            }
        }
        return result;
    }

    string smallestEquivalentString(string s1, string s2, string baseStr) {
        UnionFindDense g(256);
        for (int i = 0; i < s1.size(); ++i) {
            g.unite(s1[i], s2[i]);
        }
        auto components = g.parentToComponent();
        for (auto& [_, c]: components) {
            sort(c);
        }

        vector<int> smallest(256);
        for (int i = 0; i < 256; ++i) {
            smallest[i] = i;
        }
        for (const auto& [_, c]: components) {
            for (int i = 1; i < c.size(); ++i) {
                smallest[c[i]] = c[0];
            }
        }
        for (char& ch: baseStr) {
            ch = smallest[ch];
        }
        return baseStr;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.smallestEquivalentString("parker", "morris", "parser") << endl;
    cerr << sol.smallestEquivalentString("leetcode", "programs", "sourcecode") << endl;

    return 0;
}
