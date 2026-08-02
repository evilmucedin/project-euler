#include <string>
#include <iostream>

using namespace std;

class Solution {
public:
    string reorderSpaces(string text) {
        const int n = text.size();
        int a = 0;
        int b = n;
        while (a < n && text[a] == ' ')
            ++a;
        while (a < b && text[b - 1] == ' ')
            --b;
        return text.substr(a, b - a);

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
