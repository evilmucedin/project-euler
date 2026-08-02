#include <string>
#include <iostream>

using namespace std;

class Solution {
public:
    string reorderSpaces(string text) {
        const int n = text.size();
        string result;
        bool in = false;
        int num = 0;
        for (char ch: text) {
            if (ch >= 'a' && ch <= 'z') {
                if (!in) {
                    if (num > 0)
                        result += " ";
                    in = true;
                }
                result += ch;
                ++num;
            } else {
                in = false;
            }
        }
        return result;
    }
};

int main(void) {
    Solution s;
    // cerr << endl;
    cout << "`" << s.reorderSpaces(" practice   makes   perfect") << "'" << endl;

    return 0;
}
