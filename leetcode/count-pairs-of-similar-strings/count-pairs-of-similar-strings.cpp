#include "../header.h"

class Solution {
public:
    int similarPairs(const vector<string>& words) {
        const int n = words.size();
        vector<vector<char>> v(n);
        for (int i = 0; i < n; ++i) {
            for (char ch: words[i]) {
                v[i].emplace_back(ch);
            }
            sort(v[i].begin(), v[i].end());
            v[i].erase(unique(v[i].begin(), v[i].end()), v[i].end());
        }
        cerr << v << endl;
        int result = 0;
        int group = 1;
        for (int i = 1; i < n; ++i) {
            if (v[i] == v[i - 1]) {
                ++group;
            } else {
                result += ((group - 1)*group) / 2;
                group = 1;
            }
        }
        result += ((group - 1)*group) / 2;
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.similarPairs({"dcedceadceceaeddcedc", "dddcebcedcdbaeaaaeab", "eecbeddbddeadcbbbdbb",
                              "decbcbebbddceacdeadd", "ccbddbaedcadedbcaaae", "dddcaadaceaedcdceedd",
                              "bbeddbcbbccddcaceeea", "bdabacbbdadabbbddaea"})
         << endl;

    return 0;
}
