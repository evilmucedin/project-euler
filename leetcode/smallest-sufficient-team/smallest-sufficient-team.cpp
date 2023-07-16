#include "../header.h"

class Solution {
public:
 vector<int> smallestSufficientTeam(const vector<string>& req_skills, const vector<vector<string>>& people) {
     const int n = req_skills.size();
     const int m = people.size();

     unordered_map<string, int> skill2idx;
     for (int i = 0; i < n; ++i) {
         skill2idx[req_skills[i]] = i;
     }

     vector<int> p;
     for (const auto& sp : people) {
         int mask = 0;
         for (const auto& s : sp) {
             auto toSkill = skill2idx.find(s);
             if (toSkill != skill2idx.end()) {
                 mask = mask + (1 << toSkill->second);
             }
         }
         p.emplace_back(mask);
     }

     struct State {
         vector<int> players;
         int d{1234567890};
     };

     const int mask = (1 << n) - 1;
     vector<State> sts(mask + 1);
     sts[0].d = 0;

     using PII = pair<int, int>;
     priority_queue<PII> pq;
     pq.emplace(make_pair(0, 0));

     while (!pq.empty()) {
         auto top = pq.top();
         pq.pop();

         const auto& oldstate = sts[top.second];

         for (int i = 0; i < m; ++i) {
             int newmask = top.second | p[i];
             if (sts[newmask].d > oldstate.d + 1) {
                 sts[newmask].d = oldstate.d + 1;
                 sts[newmask].players = oldstate.players;
                 sts[newmask].players.emplace_back(i);
                 pq.emplace(make_pair(-sts[newmask].d, newmask));
             }
         }
     }

     return sts.back().players;
 }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.smallestSufficientTeam({"java", "nodejs", "reactjs"}, {{"java"}, {"nodejs"}, {"nodejs", "reactjs"}})
         << endl;

    return 0;
}
