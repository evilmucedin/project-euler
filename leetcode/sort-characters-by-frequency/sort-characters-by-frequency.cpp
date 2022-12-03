#include "../header.h"

class Solution {
public:
    string frequencySort(string s) {
        unordered_map<char, int> m;
        for (auto ch: s) {
            ++m[ch];
        }
	auto tuples = map2TuplesSwap(m);
	sortDescending(tuples);
        string s3;
        for (const auto& p : tuples) {
	    s3 += string(p.first, p.second);
        }
        return s3;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.frequencySort("cccaaa") << endl;

    return 0;
}
