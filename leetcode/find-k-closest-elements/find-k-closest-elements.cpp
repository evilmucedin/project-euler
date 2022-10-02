#include "../header.h"

#include <queue>

class Solution {
public:
    vector<int> findClosestElements(const vector<int>& arr, int k, int x) {
        using Heap = priority_queue<pair<int, int>, vector<pair<int, int>>, less<pair<int, int>>>;
        Heap h;
        for (int i = 0; i < arr.size(); ++i) {
            h.emplace(make_pair(abs(arr[i] - x), arr[i]));
            if (h.size() > k) {
                h.pop();
            }
        }
        vector<int> res(h.size());
        int pos = 0;
        while (!h.empty()) {
            res[pos++] = h.top().second;
            h.pop();
        }
        sort(res.begin(), res.end());
        return res;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findClosestElements({1, 2, 3, 4, 5}, 4, 3) << endl;

    return 0;
}
