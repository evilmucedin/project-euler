#include "../header.h"

class Solution {
public:
    bool canCross(const vector<int>& stones_) {
        unordered_set<int> stones(stones_.begin(), stones_.end());
        if (!stones.count(stones_.front() + 1)) {
            return false;
        }

        using Stone = pair<int, int>;
        using QueueItem = pair<int, Stone>;
        priority_queue<QueueItem> q;
        set<Stone> visited;
        q.emplace(QueueItem(0, make_pair(stones_.front() + 1, 1)));
        while (!q.empty()) {
            const auto tp = q.top();
            q.pop();
            // cerr << tp << endl;

            for (int v : {tp.second.second - 1, tp.second.second, tp.second.second + 1}) {
                if (v > 0 && stones.count(tp.second.first + v)) {
                    Stone s(tp.second.first + v, v);
                    if (s.first == stones_.back()) {
                        return true;
                    }
                    if (!visited.count(s)) {
                        visited.emplace(s);
                        q.emplace(QueueItem(-tp.first - 1, s));
                    }
                }
            }
        }
        return false;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.canCross({0, 1, 2, 5, 6, 9, 10, 12, 13, 14, 17, 19, 20, 21, 26, 27, 28, 29, 30}) << endl;

    return 0;
}
