#include "../header.h"

#include "lib/exception.h"

class Solution {
   public:
    using T = long long;

    static int calculate(string s) {
        string trimmed;
        for (auto ch : s) {
            if (!isspace(ch)) {
                trimmed += ch;
            }
        }

        return eval(trimmed, 0, trimmed.size());
    }

    static T eval(const string& s, int begin, int end) {
        int now = begin;
        T result = readToken(s, now, end);
        while (now < end) {
            if (s[now] == '+') {
                ++now;
                result += readToken(s, now, end);
            } else if (s[now] == '-') {
                ++now;
                result -= readToken(s, now, end);
            } else {
                THROW("Unknown operator '" << s[now] << "' " << now << "-" << end);
            }
        }
        return result;
    }

    static int findMatching(const string& s, int begin, int end) {
        int depth = 0;
        int result = begin;
        do {
            if (s[result] == '(') {
                ++depth;
            } else if (s[result] == ')') {
                --depth;
            }
            if (depth == 0 && result < end) {
                return result;
            }
            ++result;
        } while (true);

        THROW("Matching ) not found");
    }

    static T readToken(const string& s, int& now, int end) {
        if (s[now] == '-') {
            ++now;
            return -readToken(s, now, end);
        } else if (s[now] == '(') {
            int newEnd = findMatching(s, now, end);
            ++now;
            T result = eval(s, now, newEnd);
            now = newEnd + 1;
            return result;
        } else if (isdigit(s[now])) {
            T result = 0;
            while (now < end && isdigit(s[now])) {
                result = 10 * result + s[now] - '0';
                ++now;
            }
            return result;
        } else {
            THROW("Unknown character in readToken");
        }
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.calculate("1 + 1") << endl;
    cerr << sol.calculate("2-1 + 2") << endl;
    cerr << sol.calculate("(1+(4+5+2)-3)+(6+8)") << endl;

    return 0;
}
