#include "../header.h"

class Solution {
public:
    bool equationsPossible(const vector<string>& equations) {
        vector<int> component(27);

        auto mergeComponents = [&](int a, int b) {
            for (int i = 0; i < 27; ++i) {
                if (component[i] == b) {
                    component[i] = a;
                }
            }
        };

        for (int i = 0; i < 27; ++i) {
            component[i] = i;
        }
        for (const auto& s: equations) {
            if (s[1] == '=' && s[2] == '=') {
                int a = s[0] - 'a';
                int b = s[3] - 'a';
                mergeComponents(component[a], component[b]);
            }
        }
        for (const auto& s: equations) {
            if (s[1] == '!' && s[2] == '=') {
                int a = s[0] - 'a';
                int b = s[3] - 'a';
                if (component[a] == component[b]) {
                    return false;
                }
            }
        }
        return true;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.equationsPossible({"a==b","b!=a"}) << endl;
    cerr << sol.equationsPossible({"a==b","b==a"}) << endl;

    return 0;
}
