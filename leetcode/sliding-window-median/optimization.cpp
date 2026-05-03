#include <vector>
#include <set>
#include <iterator>

using namespace std;

class Solution {
public:
    vector<double> medianSlidingWindow(vector<int>& nums, int k) {
        // Use multiset to keep the window sorted: O(log k) for insertion/deletion
        multiset<int> window(nums.begin(), nums.begin() + k);

        // Iterator to the middle element (median)
        auto midIt = next(window.begin(), k / 2);

        vector<double> ans;

        // Helper to calculate current median
        auto getMedian = [&]() {
            if (k % 2 == 1) return (double)*midIt;
            // Use 0.5 * (*a + *b) to avoid integer overflow
            return ((double)*prev(midIt) + (double)*midIt) / 2.0;
        };

        for (int i = k; ; ++i) {
            ans.push_back(getMedian());

            if (i == nums.size()) break;

            // Add new element
            window.insert(nums[i]);
            if (nums[i] < *midIt) midIt--;

            // Remove old element
            auto itToRemove = window.find(nums[i - k]);
            if (*itToRemove <= *midIt) midIt++;
            window.erase(itToRemove);
        }

        return ans;
    }
};

