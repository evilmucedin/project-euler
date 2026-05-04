#include "../header.h"

#include "../../lib/minStack.h"

class Solution {
public:
    int trap(const vector<int>& height) {
        MonotonicStack<int, Max<int>> msLeft;
        MonotonicStack<int, Max<int>> msRight;
        for (int i = height.size() - 1; i >= 0; --i) {
            msRight.push(height[i]);
        }
        int ans = 0;
        for (int i = 1; i < height.size(); ++i) {
            msLeft.push(msRight.top());
            msRight.pop();
            cerr << i << " " << height[i] << " " << msLeft.getMin() << " " << msRight.getMin() << " " << msLeft.size()
                 << " " << msRight.size() << endl;
            if (!msLeft.empty() && !msRight.empty()) {
                ans += max(0, min(msLeft.getMin(), msRight.getMin()) - height[i]);
            }
        }
        return ans;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.trap({0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1}) << endl;

    return 0;
}
