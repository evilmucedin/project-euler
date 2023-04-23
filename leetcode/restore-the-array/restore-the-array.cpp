#include "../header.h"

class Solution {
public:
    vector<long long> dp_;

    static constexpr int MOD = 1e9 + 7;

    int numberOfArrays_(string s, int len, int k) {
        if (len == 0) {
            return 1;
        }
        if (len < 0) {
            return 0;
        }

        long long result = 0;
        long long power = 1;
        long long now = 0;
        bool first = true;
        for (int i = len - 1; i >= max(0, len - 10); --i) {
            if (!first) {
                power *= 10;
            }
            first = false;

            if (s[i] == '0') {
                continue;
            }
            now = now + power*(s[i] - '0');
            if (now <= k && now) {
                result += numberOfArrays(s, i, k);
            }
        }
        return result % MOD;
    }

    int numberOfArrays(string s, int len, int k) {
        if (len < 0) {
            return 0;
        }
        if (dp_[len] == -1) {
            dp_[len] = numberOfArrays_(s, len, k);
        }
        return dp_[len];
    }

    int numberOfArrays2(string s, int k) {
        dp_.clear();
        dp_.resize(s.size() + 1, -1);
        dp_[0] = 1;
        return numberOfArrays(s, s.size(), k);
    }

    int numberOfArrays(string s, int k) {
        dp_.clear();
        dp_.resize(s.size() + 1, 0);
        dp_[0] = 1;
        for (int i = 0; i <= s.size(); ++i) {
            if (i != 0 && s[i] == '0') {
                continue;
            }
            int now = 0;
            int j = i + 1;
            while (j <= s.size()) {
                now = 10 * now + s[j - 1] - '0';
                if (now <= k && now) {
                    dp_[j] = (dp_[j] + dp_[i]) % MOD;
                } else {
                    break;
                }
                ++j;
            }
        }
        // cerr << dp_ << endl;
        return dp_[s.size()];
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.numberOfArrays("1000", 10000) << endl;
    cerr << sol.numberOfArrays("1000", 10) << endl;
    cerr << sol.numberOfArrays("1317", 2000) << endl;
    cerr << sol.numberOfArrays("2020", 30) << endl;

    return 0;
}
