#include <iostream>
#include <map>

using namespace std;

class Solution {
public:
    map<int, int> m_;

    int numTrees(int n) {
        if (n == 1)
            return 1;
        auto toN = m_.find(n);
        if (toN != m_.end())
            return toN->second;
        int res = 2*numTrees(n - 1);
        for (int i = 1; i + 1 < n; ++i)
            res += numTrees(i)*numTrees(n - 1 - i);
        return res;
    }
};

int main() {
    Solution s;
    for (int i = 1; i < 20; ++i)
        cerr << i << " " << s.numTrees(i) << endl;
    return 0;
}
