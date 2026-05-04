#include "../header.h"

class Solution {
public:
    string reorganizeString(string s) {
        if (set<char>(s.begin(), s.end()).size() <= 1) {
            return "";
        }

        sort(s.begin(), s.end());
        // cerr << s << endl;

        const int n = s.size();
        string a;
        string b;
        for (int i = 0; i < n; ++i) {
            if (i < n/2) {
                a += s[i];
            } else {
                b += s[i];
            }
        }

        string result;
        int i = 0;
        int j = 0;
        while (true) {
            if (i < a.size() && j < b.size()) {
                result += a[i++];
                result += b[j++];
            } else if (i < a.size()) {
                result += a[i++];
            } else if (j < b.size()) {
                result += b[j++];
            } else {
                break;
            }
        }
        // cerr << a << " " << b << " " << "'" << result << "'" << endl;

        for (int j = 0; j < 2000000; ++j) {
            bool good = true;
            for (int i = 1; i < result.size(); ++i) {
                if (result[i - 1] == result[i]) {
                    swap(result[i - 1], result[rand() % result.size()]);
                    good = false;
                }
            }
            if (good) {
                break;
            }
        }

        for (int i = 1; i < result.size(); ++i) {
            if (result[i - 1] == result[i]) {
                return "";
            }
        }

        return result;

        /*
        vector<int> c(128);
        for (char ch: s) {
            ++c[ch];
        }

        vector<pair<int, char>> ps;
        for (int i = 0; i < 128; ++i) {
            if (c[i]) {
                ps.emplace_back(make_pair(-c[i], (char)i));
            }
        }
        sort(ps.begin(), ps.end());

        string result;
        for (int i = 0; i < 500; ++i) {
            for (auto& p: ps) {
                if (p.first) {
                    result += p.second;
                    ++p.first;
                }
            }
            if (s.size() == result.size()) {
                break;
            }
        }
        for (int j = 0; j < 2000000; ++j) {
            bool good = true;
            for (int i = 1; i < result.size(); ++i) {
                if (result[i - 1] == result[i]) {
                    swap(result[i - 1], result[rand() % result.size()]);
                    good = false;
                }
            }
            if (good) {
                break;
            }
        }
        for (int i = 1; i < result.size(); ++i) {
            if (result[i - 1] == result[i]) {
                return "";
            }
        }
        return result;
        */
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.reorganizeString("aab") << endl;
    cerr << sol.reorganizeString("aaab") << endl;

    return 0;
}
