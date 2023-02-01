#include "../header.h"

class Solution {
public:
    int bestTeamScore(const vector<int>& scores, const vector<int>& ages) {
        vector<pair<int, int>> pl;
        const int n = scores.size();
        for (int i = 0; i < n; ++i) {
            pl.emplace_back(make_pair(-ages[i], -scores[i]));
        }
        sort(pl.begin(), pl.end());
        for (int i = 0; i < n; ++i) {
            pl[i] = make_pair(-pl[i].first, -pl[i].second);
        }
        cerr << pl << endl;
        int result = 0;
        int minScore = 123456789;
        int minAge = 123456789;
        for (int i = 0; i < n; ++i) {
            if (pl[i].second <= minScore || minAge == pl[i].first) {
                cerr << "\t" << i << endl;
                result += pl[i].second;
                minScore = min(minScore, pl[i].second);
                minAge = pl[i].first;
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.bestTeamScore({4, 5, 6, 5}, {2, 1, 2, 1}) << endl;

    return 0;
}
