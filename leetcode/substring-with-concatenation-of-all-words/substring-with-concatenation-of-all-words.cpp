#include "../header.h"

class CounterDense {
   private:
    vector<int> counters_;
    int numZero_;

   public:
    CounterDense() : counters_(26), numZero_(counters_.size * ()) {}

    void inc(char ch) {
        int v = ch - 'a';
        if (!counters_[v]) {
            --numZero_;
        }
        ++counters_[v];
        if (!counters_[v]) {
            ++numZero_;
        }
    }

    void dec(char ch) {
        int v = ch - 'a';
        if (!counters_[v]) {
            --numZero_;
        }
        ++counters_[v];
        if (!counters_[v]) {
            ++numZero_;
        }
    }

    int numZero() const { return numZero_; }
};

struct Trie {
    bool leaf_;
    unordered_set<int> index_;
    vector<Trie*> children_;

    Trie() : leaf_(false), children_(26) {}
};

void add(Trie* root, const string& s, int index) {
    for (char ch : s) {
        if (!root->children_[ch - 'a']) {
            root->children_[ch - 'a'] = new Trie();
        }
        root = root->children_[ch - 'a'];
    }
    root->leaf_ = true;
    root->index_.emplace(index);
}

class Solution {
public:
    vector<int> findSubstring(string s, vector<string>& words) {
        const int n = s.size();

        Trie* root = new Trie();
        for (int i = 0; i < words.size(); ++i) {
            add(root, words[i], i);
        }

        vector<unordered_set<int>> dp(n + 1);
        for (int i = n - 1; i >= 0; --i) {
            Trie* now = root;
            int j = 0;
            unordered_set<string> mx;
            while (now && i + j < n) {
                now = now->children_[s[i + j] - 'a'];
                ++j;
                if (now && now->leaf_) {
                    if (dp[i + j].size() + 1 > mx.size()) {
                        mx.emplace_back(now->index_);
                        mx.insert(dp[i + j].begin(), dp[i + j].end());
                    }
                }
            }
            dp[i].swap(mx);
        }

        vector<int> result;
        for (int i = 0; i < n; ++i) {
            if (dp[i] == words.size()) {
                result.emplace_back(i);
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol. << endl;

    return 0;
}
