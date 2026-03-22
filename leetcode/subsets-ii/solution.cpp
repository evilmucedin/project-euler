#include <vector>
#include <iostream>
#include <algorithm>
#include <set>

using namespace std;

class Solution {
    public:
        static void add(set<vector<int>>& s, int n, vector<int>& m, int i, const vector<int>& nums) {
            s.insert(m);
            if (i < n) {
                add(s, n, m, i + 1, nums);
                m.push_back(nums[i]);
                add(s, n, m, i + 1, nums);
                m.pop_back();
            }
        }

        vector<vector<int>> subsetsWithDup(vector<int>& nums) {
            sort(nums.begin(), nums.end());
            set<vector<int>> s;
            const int n = nums.size();
            vector<int> m;
            add(s, n, m, 0, nums);
            /*
            for (int i = 0; i < n; ++i) {
                vector<int> m;
                for (int j = i; j < n; ++j) {
                    s.insert(m);
                    m.push_back(nums[j]);
                }
                s.insert(m);
            }
            */

            vector<vector<int>> result;
            for (const auto& m: s)
                // if (m.size() != n)
                result.push_back(m);
            return result;
        }
};

int main() {
    Solution s;
    // cerr << s.subsetsWithDup({1,2,2}) << endl;
    return 0;
}
