#include "../header.h"

#include <set>

class Solution {
   public:
    vector<int> maxPoints(const vector<vector<int>>& grid, const vector<int>& queries) {
        const int n = grid.size();
        const int m = grid[0].size();
        const int k = queries.size();

        vector<pair<int, int>> qs(k);
        for (int i = 0; i < k; ++i) {
            qs[i] = make_pair(queries[i], i);
        }
        sort(qs.begin(), qs.end());

        vector<int> result(k);
        vector<vector<bool>> used(n, vector<bool>(m));
        // vector<vector<int>> nb(n, vector<int>(m));
        vector<pair<int, int>> qfull;
        vector<pair<int, int>> newqf;
        qfull.emplace_back(make_pair(0, 0));
        int filled = 0;
        static constexpr int DIRS[] = {1, 0, 0, 1, -1, 0, 0, -1};
        for (int i = 0; i < k; ++i) {
            queue<pair<int, int>> q;
            for (const auto& p : qfull) {
                // if (nb[p.first][p.second] != 4) {
                    q.emplace(p);
                // }
            }
            while (!q.empty()) {
                const auto top = q.front();
                q.pop();
                if (grid[top.first][top.second] < qs[i].first) {
                    if (!used[top.first][top.second]) {
                        used[top.first][top.second] = true;
                        ++filled;
                        for (int j = 0; j < 4; ++j) {
                            auto np = top;
                            np.first += DIRS[2 * j];
                            np.second += DIRS[2 * j + 1];
                            if (np.second >= 0 && np.second < m && np.first >= 0 && np.first < n &&
                                !used[np.first][np.second]) {
                                // ++nb[top.first][top.second];
                                q.emplace(np);
                                qfull.emplace_back(np);
                            }
                        }
                    }
                }
            }
            result[qs[i].second] = filled;

            /*
            newqf.clear();
            for (const auto& top : qfull) {
                if (nb[top.first][top.second] != 4) {
                    newqf.emplace_back(top);
                }
            }
            newqf.swap(qfull);
            */
            /*
            newqf.clear();
            for (const auto& top : qfull) {
                int count = 0;
                for (int j = 0; j < 4; ++j) {
                    auto np = top;
                    np.first += DIRS[2 * j];
                    np.second += DIRS[2 * j + 1];
                    if (np.second >= 0 && np.second < m && np.first >= 0 && np.first < n &&
                        !used[np.first][np.second]) {
                        ++count;
                        break;
                    }
                }
                if (count) {
                    newqf.emplace(top);
                }
            }
            newqf.swap(qfull);
            */
            sort(qfull.begin(), qfull.end());
            qfull.erase(unique(qfull.begin(), qfull.end()), qfull.end());
            // cerr << vector<pair<int, int>>(ITRANGE(newqf)) << endl;
            /*
            */
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    // [85,145,166,171,0,1,0,171,126]
    cerr << sol.maxPoints(
                {{249472, 735471, 144880, 992181, 760916, 920551, 898524, 37043, 422852, 194509, 714395, 325171},
                 {295872, 922051, 900801, 634980, 644237, 912433, 857189, 98466, 725226, 984534, 370121, 399006},
                 {618420, 573065, 587011, 298153, 694872, 12760, 880413, 593508, 474772, 291113, 852444, 77998},
                 {67650, 426517, 146447, 190319, 379151, 184754, 479219, 106819, 138473, 865661, 799297, 228827},
                 {390392, 789371, 772048, 730506, 7144, 862164, 650590, 21524, 879440, 396198, 408897, 851020},
                 {932044, 662093, 436861, 246956, 128943, 167432, 267483, 148325, 458128, 418348, 900594, 831373},
                 {742255, 795191, 598857, 441846, 243888, 777685, 313717, 560586, 257220, 488025, 846817, 554722},
                 {252507, 621902, 87704, 599753, 651175, 305330, 620166, 631193, 385405, 183376, 432598, 706692},
                 {984416, 996917, 586571, 324595, 784565, 300514, 101313, 685863, 703194, 729430, 732044, 349877},
                 {155629, 290992, 539879, 173659, 989930, 373725, 701670, 992137, 893024, 455455, 454886, 559081},
                 {252809, 641084, 632837, 764260, 68790, 732601, 349257, 208701, 613650, 429049, 983008, 76324},
                 {918085, 126894, 909148, 194638, 915416, 225708, 184408, 462852, 40392, 964501, 436864, 785076},
                 {875475, 442333, 111818, 494972, 486734, 901577, 46210, 326422, 603800, 176902, 315208, 225178},
                 {171174, 458473, 744971, 872087, 680060, 95371, 806370, 322605, 349331, 736473, 306720, 556064},
                 {207705, 587869, 129465, 543368, 840821, 977451, 399877, 486877, 327390, 8865, 605705, 481076}},
                {690474, 796832, 913701, 939418, 46696, 266869, 150594, 948153, 718874})
         << endl;

    return 0;
}
