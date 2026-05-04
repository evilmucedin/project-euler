#include "../header.h"

class Solution {
public:
 int jobScheduling(const vector<int>& startTime0, const vector<int>& endTime0, const vector<int>& profit0) {
     auto startTime = startTime0;
     auto endTime = endTime0;
     auto profit = profit0;

     vector<int> ts;
     ts.emplace_back(0);
     for (auto t : startTime) {
         ts.emplace_back(t);
     }
     for (auto t : endTime) {
         ts.emplace_back(t);
     }
     sort(ts.begin(), ts.end());
     ts.erase(unique(ts.begin(), ts.end()), ts.end());
     unordered_map<int, int> t2index;
     for (int i = 0; i < ts.size(); ++i) {
         t2index[ts[i]] = i;
     }
     for (auto& t : startTime) {
         t = t2index[t];
     }
     for (auto& t : endTime) {
         t = t2index[t];
     }
     unordered_map<int, vector<int>> end2index;
     for (int i = 0; i < endTime.size(); ++i) {
         end2index[endTime[i]].emplace_back(i);
     }
     vector<int> maxProfit(ts.size(), -1);
     maxProfit[0] = 0;
     for (int i = 1; i < ts.size(); ++i) {
         maxProfit[i] = maxProfit[i - 1];
         const auto& jobs = end2index[i];
         for (int j = 0; j < jobs.size(); ++j) {
             maxProfit[i] = max(maxProfit[i], maxProfit[startTime[jobs[j]]] + profit[jobs[j]]);
         }
     }
     return maxProfit.back();
 }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.jobScheduling({1, 2, 3, 3}, {3, 4, 5, 6}, {50, 10, 40, 70}) << endl;

    return 0;
}
