#include "../header.h"

class Solution {
public:
    string minWindow(string s, string t) {
        if (s.empty() || t.empty()) {
            return "";
        }
        unordered_map<char, int> ch2count0;
        for (char ch: t) {
            ++ch2count0[ch];
        }
        auto ch2count = ch2count0;
        int left = 0;
        int right = 0;
        static constexpr int INF = 123456890;
        int min = INF;
        int minLeft = 0;
        int minRight = 0;
        int nonZero = ch2count.size();
        while (right < s.size()) {
            // cerr << "!" << left << " " << right << endl;
            // auto ch2count = ch2count0;
            while ((right < s.size()) && nonZero) {
                char ch = s[right];
                ++right;
                auto toCh = ch2count.find(ch);
                if (toCh != ch2count.end()) {
                    --toCh->second;
                    if (toCh->second == 0) {
                        --nonZero;
                    }
                }
            }

            if (nonZero == 0) {
                while (left < right) {
                    char ch = s[left];
                    auto toCh = ch2count.find(ch);
                    if (toCh == ch2count.end()) {
                        ++left;
                    } else if (toCh->second == 0) {
                        break;
                    } else {
                        ++toCh->second;
                        ++left;
                    }
                }
                // cerr << "?" << left << " " << right << endl;
                if (right - left < min) {
                    min = right - left;
                    minLeft = left;
                    minRight = right;
                }
            }

            char ch = s[left];
            auto toCh = ch2count.find(ch);
            if (toCh != ch2count.end()) {
                if (toCh->second == 0) {
                    ++toCh->second;
                    ++nonZero;
                } else {
                    ++toCh->second;
                }
            }
            ++left;
        }

        return s.substr(minLeft, minRight - minLeft);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minWindow("ab", "a") << endl;
    cerr << sol.minWindow("ADOBECODEBANC", "ABC") << endl;
    cerr << sol.minWindow("bdab", "ab") << endl;

    return 0;
}
