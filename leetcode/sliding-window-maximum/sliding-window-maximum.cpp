#include "../header.h"

struct MonoStack {
    vector<int> data_;
    vector<int> maxStack_;

    void push(int val) {
        data_.push_back(val);
        if (maxStack_.empty() || val >= maxStack_.back()) {
            maxStack_.push_back(val);
        }
    }

    int pop() {
        int val = data_.back();
        data_.pop_back();
        if (maxStack_.back() == val) {
            maxStack_.pop_back();
        }
        return val;
    }

    int max() const {
        return maxStack_.back();
    }

    bool empty() const {
        return data_.empty();
    }
};

ostream& operator<<(ostream& s, const MonoStack& st) {
    s << st.data_;
    return s;
}

struct MonoQueue {
    MonoStack a_;
    MonoStack b_;

    void push(int val) {
        a_.push(val);
    }

    int pop() {
        if (b_.empty()) {
            while (!a_.empty()) {
                b_.push(a_.pop());
            }
        }
        return b_.pop();
    }

    int max() const {
        if (!a_.empty() && !b_.empty()) {
            return std::max<int>(a_.max(), b_.max());
        } else if (!a_.empty()) {
            return a_.max();
        } else {
            return b_.max();
        }
    }
};

class Solution {
public:
    vector<int> maxSlidingWindow(const vector<int>& nums, int k) {
        MonoQueue q;
        for (int i = 0; i < k; ++i) {
            q.push(nums[i]);
        }

        vector<int> result;
        for (int i = k; i < nums.size(); ++i) {
            // cerr << q.a_ << " " << q.b_ << endl;
            result.emplace_back(q.max());
            q.pop();
            q.push(nums[i]);
        }
        result.emplace_back(q.max());

        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxSlidingWindow({1, 3, -1, -3, 5, 3, 6, 7}, 3) << endl;

    return 0;
}
