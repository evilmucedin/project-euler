#include "../header.h"

class Solution {
public:
    int maxEvents(const vector<vector<int>>& events) {
        if (events.empty()) {
            return 0;
        }

        auto evs = events;
        sort(evs.begin(), evs.end());

        int result = 0;
        int d = evs[0][0];
        int i = 0;

        priority_queue<int> pq;
        while (true) {
            while (i < evs.size() && evs[i][0] <= d) {
                pq.emplace(-evs[i][1]);
                ++i;
            }

            if (i == evs.size() && pq.empty()) {
                break;
            }

            // cerr << d << " " << result << endl;

            if (!pq.empty()) {
                int top = -pq.top();
                pq.pop();
                if (d <= top) {
                    // cerr << "\t" << d << " " << result << " " << top << endl;
                    ++result;
                    ++d;
                }
            } else {
                ++d;
            }

            if (pq.empty() && i < evs.size()) {
                d = max(d, evs[i][0]);
            }
        }

        return result;
    }
};


int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxEvents({{1, 10}, {2, 2}, {2, 2}, {2, 2}, {2, 2}}) << endl;

    return 0;
}
