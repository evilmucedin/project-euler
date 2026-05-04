#include "../header.h"

class Solution {
   public:
    static bool isPalindrome2(const string& a, const string& b, size_t offset) {
        const int len = a.size() + b.size();

        auto get = [&](int i) {
            if (i < a.size()) {
                return a[i];
            } else {
                return b[i - a.size()];
            }
        };

        if (get(len / 5) != get(len - len / 5 - 1)) {
            return false;
        }

        if (get(len / 3) != get(len - len / 3 - 1)) {
            return false;
        }

        if (get(len / 4) != get(len - len / 4 - 1)) {
            return false;
        }

        for (int i = offset; i + i + offset < len; ++i) {
            if (get(i) != get(len - i - 1)) {
                return false;
            }
        }
        return true;
    }

    static string reverse(const string& s) {
        auto res = s;
        std::reverse(res.begin(), res.end());
        return res;
    }

    static string commonPrefix(const string& a, const string& b) {
        string result;
        for (size_t i = 0; i < a.size(); ++i) {
            if (i == b.size()) {
                return result;
            }
            if (a[i] == b[i]) {
                result += a[i];
            } else {
                return result;
            }
        }
        return result;
    }

    static bool isPalindrome(const string& s) {
        for (size_t i = 0; i + i < s.size(); ++i) {
            if (s[i] != s[s.size() - i - 1]) {
                return false;
            }
        }
        return true;
    }

    static bool isPalindromePart(const string& s, size_t left, size_t right) {
        const char* l = s.data() + left;
        const char* r = s.data() + right - 1;
        while (l < r) {
            if (*l != *r) {
                return false;
            }
            ++l;
            --r;
        }
        /*
        for (size_t i = 0; left + i + i < right; ++i) {
            if (s[left + i] != s[right - 1 - i]) {
                return false;
            }
        }
        */
        return true;
    }

    using Substrings = vector<pair<int, int>>;

    static Substrings findAllPalindromes(const string& s) {
        Substrings result;
        auto begin = s.data();
        auto end = s.data() + s.size() - 1;

        auto addAll = [&](const char* l, const char* r) {
            while (l >= begin && r <= end) {
                if (*l == *r) {
                    result.emplace_back(make_pair(l - begin, r - begin));
                    --l;
                    ++r;
                } else {
                    return;
                }
            }
        };

        for (int i = 0; i < s.size(); ++i) {
            addAll(begin + i, begin + i);
            addAll(begin + i, begin + i + 1);
        }

        return result;
    }

    vector<vector<int>> palindromePairs(vector<string>& words) {
        unordered_map<string, int> revWord2Index;
        revWord2Index.reserve(words.size());
        int emptyIndex;
        bool hasEmpty = false;
        for (int i = 0; i < words.size(); ++i) {
            if (words[i].empty()) {
                hasEmpty = true;
                emptyIndex = i;
            } else {
                revWord2Index[reverse(words[i])] = i;
            }
        }

        vector<vector<int>> result;
        for (int i = 0; i < words.size(); ++i) {
            const auto& w = words[i];

            if (isPalindrome(w) && !w.empty() && hasEmpty) {
                result.emplace_back(vector<int>{i, emptyIndex});
                result.emplace_back(vector<int>{emptyIndex, i});
            }

            // const auto wPalindromes = findAllPalindromes(w);

            for (size_t pos = 0; pos < w.size(); ++pos) {
                if (isPalindromePart(w, pos, w.size())) {
                    const auto left = w.substr(0, pos);
                    auto toRev = revWord2Index.find(left);
                    if (toRev != revWord2Index.end() && i != toRev->second) {
                        result.emplace_back(vector<int>{i, toRev->second});
                    }
                }
                if (isPalindromePart(w, 0, pos)) {
                    const auto right = w.substr(pos, w.size());
                    auto toRev = revWord2Index.find(right);
                    if (toRev != revWord2Index.end() && i != toRev->second) {
                        result.emplace_back(vector<int>{toRev->second, i});
                    }
                }
            }
        }

        sort(result.begin(), result.end());

        return result;
    }

    vector<vector<int>> palindromePairsSlow(vector<string>& words) {
        string maxCommonPrefix = words[0];
        for (size_t i = 1; i < words.size(); ++i) {
            maxCommonPrefix = commonPrefix(maxCommonPrefix, words[i]);
        }

        string maxCommonSuffix = reverse(words[0]);
        for (size_t i = 1; i < words.size(); ++i) {
            maxCommonSuffix = commonPrefix(maxCommonSuffix, reverse(words[i]));
        }
        string maxCommon = commonPrefix(maxCommonPrefix, maxCommonSuffix);

        vector<vector<int>> charToIndices(128);
        vector<int> empty;
        for (int i = 0; i < words.size(); ++i) {
            if (!words[i].empty()) {
                charToIndices[words[i].back()].emplace_back(i);
            } else {
                empty.emplace_back(i);
            }
        }
        vector<vector<int>> result;
        for (int i = 0; i < words.size(); ++i) {
            if (words[i] == reverse(words[i]) && !words[i].empty()) {
                for (auto j : empty) {
                    if (i == j) {
                        continue;
                    }
                    result.emplace_back(vector<int>{i, j});
                    result.emplace_back(vector<int>{j, i});
                }
            }
            for (auto j : charToIndices[words[i].front()]) {
                if (i == j) {
                    continue;
                }
                if (isPalindrome2(words[i], words[j], maxCommon.size())) {
                    result.emplace_back(vector<int>{i, j});
                }
            }
        }
        return result;
    }

    int countSubstrings(string s) { return findAllPalindromes(s).size(); }
};

int main() {
    Solution sol;
    Timer t("Pairs timer");
    cerr << sol.countSubstrings("abc") << endl;
    cerr << sol.countSubstrings("aaa") << endl;

    return 0;
}
