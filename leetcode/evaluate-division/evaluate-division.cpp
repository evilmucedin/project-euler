#include "../header.h"

class Solution {
   public:
    vector<double> calcEquation(const vector<vector<string>>& equations, const vector<double>& values,
                                const vector<vector<string>>& queries) {
        unordered_map<string, int> key2value;

        auto add = [&](const string& s) {
            if (!key2value.count(s)) {
                key2value[s] = key2value.size();
            }
        };

        for (const auto& e: equations) {
            add(e[0]);
            add(e[1]);
        }
        for (const auto& e: queries) {
            add(e[0]);
            add(e[1]);
        }

        vector<vector<double>> mt(26, vector<double>(26, -1));

        for (int i = 0; i < equations.size(); ++i) {
            const auto& e = equations[i];
            mt[key2value[e[0]]][key2value[e[1]]] = values[i];
            if (values[i]) {
                mt[key2value[e[1]]][key2value[e[0]]] = 1.0 / values[i];
            }
        }
        /*
        for (int i = 0; i < 26; ++i) {
            mt[i][i] = 1.0;
        }
        */

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 26; ++j) {
                for (int k = 0; k < 26; ++k) {
                    for (int l = 0; l < 26; ++l) {
                        if (mt[j][l] == -1) {
                            if (mt[j][k] != -1 && mt[k][l] != -1) {
                                mt[j][l] = mt[j][k] * mt[k][l];
                            }
                        }
                    }
                }
            }
        }

        const int m = queries.size();
        vector<double> result(m);
        for (int i = 0; i < m; ++i) {
            const auto& q = queries[i];
            result[i] = mt[key2value[q[0]]][key2value[q[1]]];
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.calcEquation({{"a", "b"}, {"b", "c"}}, {2.0, 3.0},
                             {{"a", "c"}, {"b", "a"}, {"a", "e"}, {"a", "a"}, {"x", "x"}})
         << endl;

    return 0;
}
