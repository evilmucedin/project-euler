#include "../header.h"

class Solution {
public:
    map<string, long long int> cache_;

    int numDecodings(const string& s) {
        auto toS = cache_.find(s);
        if (toS == cache_.end()) {
            auto res = numDecodings_(s);
            cache_.emplace(s, res);
            return res;
        }
        return toS->second;
    }

    int numDecodings_(const string& s) {
        if (s.empty()) {
            return 1;
        }
        if (s[0] == '0') {
            return 0;
        }
        long long int res = numDecodings(s.substr(1));
        if (s.size() > 1) {
            int d2 = 10 * (s[0] - '0') + s[1] - '0';
            if (d2 <= 26 && d2 >= 1 && s[0] != '0') {
                res += numDecodings(s.substr(2));
            }
        }
        return res;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.numDecodings("12") << endl;
    cerr << sol.numDecodings("226") << endl;
    cerr << sol.numDecodings("06") << endl;

    return 0;
}
