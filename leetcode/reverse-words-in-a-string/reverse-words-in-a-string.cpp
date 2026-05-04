#include "../header.h"

class Solution {
public:
    string reverseWords(string s) {
        if (s.empty()) {
            return "";
        }
        int begin = 0;
        int now = 0;
        bool in = false;
        vector<string> vw;
        while (now <= s.size()) {
            const bool good = isalpha(s[now]);
            if (in) {
                if (!good) {
                    vw.emplace_back(string(s.begin() + begin, s.begin() + now));
                    in = false;
                }
            } else {
                if (good) {
                    begin = now;
                    in = true;
                }
            }
            ++now;
        }
        string result;
        for (auto i = vw.rbegin(); i != vw.rend(); ++i) {
            if (result.size()) {
                result += " ";
            }
            result += *i;
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.reverseWords("the sky is blue") << endl;

    return 0;
}
