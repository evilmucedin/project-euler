#include "../header.h"

class Solution {
public:
    string orderlyQueue(string s, int k) {
        if (k <= 0) {
            return s;
        }

        if (k > 1) {
            sort(s.begin(), s.end());
            return s;
        }

        string mn = s;
        const int n = s.size();
        s += s;
        for (int i = 0; i < n; ++i) {
            mn = min(mn, s.substr(i, n));
        }
        return mn;

        /*
        priority_queue<pair<char, int>> q;
        for (int i = 0; i < k; ++i) {
            q.emplace(make_pair(s[i], i));
        }
        int move = 0;
        while (true) {
            const auto tp = q.top();
            // cerr << "!" << tp << endl;
            q.pop();
            if ((tp.second + 1 < s.size()) && (s[tp.second + 1] < tp.first)) {
                s += tp.first;
                s[tp.second] = '#';
            } else {
                break;
            }
            if (move + k >= s.size()) {
                break;
            }
            q.emplace(make_pair(s[move + k], move + k));
            ++move;
        }
        string result;
        for (char ch: s) {
            if (ch != '#') {
                result += ch;
            }
        }
        return result;
        */
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.orderlyQueue("cba", 1) << endl;
    cerr << sol.orderlyQueue("baaca", 3) << endl;
    cerr << sol.orderlyQueue("v", 1) << endl;
    cerr << sol.orderlyQueue("kuh", 1) << endl;

    return 0;
}
