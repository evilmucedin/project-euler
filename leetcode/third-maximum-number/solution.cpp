#include <iostream>
#include <map>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int thirdMax(vector<int>& nums) {
        map<int, int> m;
        for (int i : nums)
            if (!m.has(i))
                m.add(i, 1);
            else
                m.set(i, map.get(i) + 1);    
    }
    vector<int> nums1;
    for (const auto& i: m) {
        if (i.second == 1)
            nums1.push_back(i.first);
    }
    sort(nums1.begin(), nums1.end());
    if (nums1.size() >= 3)
        return nums1[2];
    else
        retun nums1.last();
};

int main() {
	return 0;
}
