#include "../header.h"

class Solution {
public:
    unordered_map<string, int> cache_;

    int can_(const unordered_set<string>& set, const string& word) {
        if (word.empty()) {
            return 0;
        }

        string prefix;
        int mx = 0;
        for (int i = 0; i < word.size(); ++i) {
            prefix += word[i];
            if (set.count(prefix)) {
                auto res = can(set, word.substr(i + 1));
                if (res <= word.size()) {
                    mx = max(mx, 1 + res);
                }
            }
        }

        if (mx == 0 || mx > word.size()) {
            return 1000000000;
        }

        return mx;
    }

    int can(const unordered_set<string>& set, const string& word) {
        auto toWord = cache_.find(word);
        if (toWord == cache_.end()) {
            auto result = can_(set, word);
            cache_.emplace(word, result);
            return result;
        }
        return toWord->second;
    }

    vector<string> findAllConcatenatedWordsInADict(const vector<string>& words) {
        unordered_set<string> s(words.begin(), words.end());
        cache_.clear();

        vector<string> result;
        for (const string& word: words) {
            auto res = can(s, word);
            if (res > 1 && res <= word.size()) {
                result.emplace_back(word);
            }
        }

        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findAllConcatenatedWordsInADict(
                {"cat", "cats", "catsdogcats", "dog", "dogcatsdog", "hippopotamuses", "rat", "ratcatdogcat"})
         << endl;
    cerr << sol.findAllConcatenatedWordsInADict(
                {"a", "b", "ab", "abc"})
         << endl;

    return 0;
}
