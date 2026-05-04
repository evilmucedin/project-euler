#include "../header.h"

class Solution {
public:
    string pushDominoes(const string& dominoes) {
        string result;
        result.resize(dominoes.size());
        for (int i = 0; i < dominoes.size(); ++i) {
            if (dominoes[i] == '.') {
                static constexpr int INF = 1000000;
                int dr = INF;
                int dl = INF;
                int j = i;
                while (j >= 0 && dominoes[j] == '.') {
                    --j;
                }
                if (j >= 0 && dominoes[j] == 'R') {
                    dr = i - j;
                }
                j = i;
                while (j < dominoes.size() && dominoes[j] == '.') {
                    ++j;
                }
                if (j < dominoes.size() && dominoes[j] == 'L') {
                    dl = j - i;
                }
                if (dr != INF && dr < dl) {
                    result[i] = 'R';
                } else if (dl != INF && dl < dr) {
                    result[i] = 'L';
                } else {
                    result[i] = '.';
                }
            } else {
                result[i] = dominoes[i];
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.pushDominoes(".L.R...LR..L..") << endl;

    return 0;
}
