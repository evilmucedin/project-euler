#include "../header.h"

class WordCounter {
private:
public:
    unordered_map<string, int> counters_;
    int numZeroes_{};

public:
    void inc(const string& s) {
        ++counters_[s];
    }

    void add(const string& s) {
        if (counters_.count(s)) {
            if (!counters_[s]) {
                --numZeroes_;
            }
        }
        --counters_[s];
        if (!counters_[s]) {
            ++numZeroes_;
        }
    }

    void remove(const string& s) {
        if (counters_.count(s)) {
            if (!counters_[s]) {
                --numZeroes_;
            }
        }
        ++counters_[s];
        if (!counters_[s]) {
            ++numZeroes_;
        }
    }

    int size() const {
        return counters_.size();
    }

    int numZeroes() const {
        return numZeroes_;
    }
};

class Solution {
public:
    vector<int> findSubstring(const string& s, const vector<string>& words) {
        const int n = s.size();
        const int k = words[0].size();

        WordCounter wc0;
        for (const auto& s: words) {
            wc0.inc(s);
        }
        const int m = wc0.size();

        if (n < words.size()*k) {
            return {};
        }

        vector<int> result;
        for (int i = 0; i < k; ++i) {
            vector<string> parts;
            int j = 0;
            while (i + (j + 1)*k <= n) {
                parts.emplace_back(s.substr(i + j*k, k));
                ++j;
            }

            if (parts.size() < words.size()) {
                continue;
            }

            auto wc = wc0;
            for (int j = 0; j < words.size(); ++j) {
                wc.add(parts[j]);
            }

            if (wc.numZeroes() == wc.size()) {
                result.emplace_back(i);
            }

            for (int j = words.size(); j < parts.size(); ++j) {
                wc.remove(parts[j - words.size()]);
                wc.add(parts[j]);

                cerr << i << " " << j << " " << wc.counters_ << " " << wc.numZeroes() << endl;

                if (wc.numZeroes() == wc.size()) {
                    result.emplace_back(i + (j - words.size() + 1)*k);
                }
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findSubstring("barfoothefoobarman", {"foo", "bar"}) << endl;
    cerr << sol.findSubstring("wordgoodgoodgoodbestword", {"word", "good", "best", "good"}) << endl;
    cerr << sol.findSubstring("ababababab", {"ababa", "babab"}) << endl;

    return 0;
}
