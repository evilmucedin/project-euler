#include "../header.h"

class Solution {
public:
    using CacheKey = pair<int, int>;
    using Cache = map<CacheKey, bool>;
    Cache cache_;

    bool isMatch(const string& s, const string& p) {
        cache_.clear();
        return isMatchSuffix(s, 0, p, 0);
    }

    bool isMatchSuffix(const string& s, int bs, const string& p, int bp) {
        CacheKey key = make_pair(bs, bp);
        auto toKey = cache_.find(key);
        if (toKey != cache_.end()) {
            return toKey->second;
        }
        const bool result = isMatchSuffix_(s, bs, p, bp);
        // cerr << "'" << s << ":" << bs << "' '" << p << ":" << bp << "'= " << result << endl;
        cache_.emplace(key, result);
        return result;
    }

    bool isMatchSuffix_(const string& s, int bs, const string& p, int bp) {
        if (bp == p.size()) {
            return s.size() == bs;
        }
        if (p[bp] == '?') {
            if (s.size() == bs) {
                return false;
            }
            return isMatchSuffix_(s, bs + 1, p, bp + 1);
        } else if (p[bp] == '*') {
            for (int i = bs; i <= s.size(); ++i) {
                // cerr << i << " " << s.size() << endl;
                if (isMatchSuffix(s, i, p, bp + 1)) {
                    return true;
                }
            }
            return false;
        }
        if (s.size() == bs) {
            return false;
        }
        if (s[bs] != p[bp]) {
            return false;
        }
        if (bs + 1 <= s.size() && bp + 1 <= p.size()) {
            return isMatchSuffix(s, bs + 1, p, bp + 1);
        } else {
            return true;
        }
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.isMatch("aa", "a") << endl;
    cerr << sol.isMatch("aa", "*") << endl;
    cerr << sol.isMatch("cb", "?a") << endl;
    cerr << sol.isMatch("mississippi", "m??*ss*?i*pi") << endl;

    return 0;
}
