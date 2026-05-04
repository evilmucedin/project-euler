#include "../header.h"

class Solution {
   public:
    int longestValidParentheses(const string& s) {
        int result = 0;
        for (int i = 0; i < s.size(); ++i) {
            int depth = 0;
            for (int j = i; depth >= 0 && j < s.size(); ++j) {
                if (s[j] == '(') {
                    ++depth;
                } else if (s[j] == ')') {
                    --depth;
                }
                if (depth == 0) {
                    result = max(result, j - i + 1);
                }
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.longestValidParentheses(")()())") << endl;

    return 0;
}
