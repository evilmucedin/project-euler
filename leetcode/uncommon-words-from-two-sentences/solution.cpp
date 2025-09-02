#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

class Solution {
public:
    vector<string> uncommonFromSentences(string s1, string s2) {
        const string s = s1 + " " + s2;
        istringstream is(s);
        string ss;
        unordered_map<string, int> mp;
        while (is >> ss) {
            ++mp[ss];
        }
        vector<string> result;
        for (const auto m: mp)
            if (m.second == 1)
                result.push_back(m.first);
        return result;
    }
};

int main() {
    Solution s;
    return 0;
}
