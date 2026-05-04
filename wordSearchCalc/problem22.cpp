#include <vector>
#include <string>

using namespace std;

class Solution {
public:
    vector<string> generateParenthesis(int n) {
        vector<string> res;
        if (n == 0) {
            
        } else if (n == 1) {
            res.push_back("()");
        } else {
            string ss1, ss2;
            for (int i = 0; i < n; ++i) {
                ss1 += "(";
                ss2 += ")";

            res.push_back(ss1 + ss2);
            for (int i = n - 1; i > 0; --i) {
                string s1, s2;
                for (int j = 0; j < i; ++j) {
                    s1 += "(";
                    s2 += ")";
                }
                const auto ii = generateParenthesis(n - i);
                for (auto s3 = ii.begin(); s3 != ii.end(); ++s3) {
                    res.push_back(s1 + *s3 + s2);
                }            
            }
            :q}
            return res;
            }
        };

int main() {
	Solution s;
	s.generateParenthesis(3);
	return 0;
}
