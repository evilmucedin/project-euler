#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

class Solution {
public:
    int uniqueMorseRepresentations(vector<string>& words) {
        static const vector<string> MORSE{".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--.."};
        cout << MORSE.size() << endl;
        map<char, string> m;
        for (int i = 0; i < 26; ++i)
            m.insert({char('a' + i), MORSE[i]});
        set<string> result;
        for (const auto& s: words) {
            string ss;
            for (char ch: s)
                ss += m[ch];
            result.insert(ss);
        }
        return result.size();
    }
};

int main() {
    Solution s;
    vector<string> v1{"gin","zen","gig","msg"};
    cout << s.uniqueMorseRepresentations(v1) << endl;
    return 0;
}
