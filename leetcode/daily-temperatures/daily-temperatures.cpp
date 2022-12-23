#include <set>

#include "../header.h"

class Solution {
   public:
    struct Position {
        int temp_;
        int index_;

        bool operator<(const Position& rhs) const {
            if (temp_ != rhs.temp_) {
                return temp_ < rhs.temp_;
            }
            return index_ < rhs.index_;
        }
    };

    vector<int> dailyTemperatures(const vector<int>& temperatures) {
        const int n = temperatures.size();
        vector<int> result(n);
        if (set<int>(temperatures.begin(), temperatures.end()).size() == 1) {
            return result;
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 1; j < 50; ++j) {
                if (i + j < n) {
                    if (temperatures[i + j] > temperatures[i]) {
                        result[i] = j;
                        break;
                    }
                }
            }
        }

        vector<Position> pairs(n);
        for (int i = 0; i < n; ++i) {
            pairs[i] = {temperatures[i], i};
        }
        sort(pairs.begin(), pairs.end());
        set<pair<int, int>> indices;
        int lastAdded = n - 1;
        for (int i = n - 1; i >= 0; --i) {
            if (!result[pairs[i].index_]) {
                auto toPos = indices.upper_bound(make_pair(pairs[i].index_, 0));
                if (toPos == indices.end()) {
                    result[pairs[i].index_] = 0;
                } else {
                    result[pairs[i].index_] = toPos->first - pairs[i].index_;
                }
            }
            while (lastAdded >= 0 && (lastAdded >= i) && (pairs[lastAdded].temp_ > pairs[i].temp_)) {
                indices.emplace(make_pair(pairs[lastAdded].index_, pairs[lastAdded].temp_));
                --lastAdded;
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.dailyTemperatures({73, 74, 75, 71, 69, 72, 76, 73}) << endl;

    return 0;
}
