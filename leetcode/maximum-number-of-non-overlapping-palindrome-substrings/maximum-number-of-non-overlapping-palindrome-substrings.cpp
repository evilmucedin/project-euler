#include "../header.h"

class Solution {
public:
    static bool isPalindrome(const string& s, int begin, int end) {
        if (begin < 0) {
            return false;
        }
        if (end < 0) {
            return false;
        }
        if (end > s.size()) {
            return false;
        }
        while (begin < end) {
            if (s[begin] != s[end - 1]) {
                return false;
            }
            ++begin;
            --end;
        }
        return true;
    }

    int maxPalindromes(string s, int k) {
        if (s.size() < k) {
            return 0;
        }
        for (int j = k; j <= s.size(); ++j) {
            for (int i = min(j - k, 0); i + k <= j; ++i) {
                if (isPalindrome(s, i, j)) {
                    // cerr << s << " " << i << " " << j << endl;
                    return 1 + maxPalindromes(s.substr(j), k);
                }
            }
        }
        return 0;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxPalindromes("abaccdbbd", 3) << endl;
    cerr << sol.maxPalindromes("adbcda", 2) << endl;

    return 0;
}
