#include "../header.h"

class MyCalendarThree {
public:
    vector<pair<int, int>> data_;

    MyCalendarThree() {

    }

    int book(int start, int end) {
        data_.emplace_back(make_pair(start, 1));
        data_.emplace_back(make_pair(end, 0));
        sort(data_.begin(), data_.end());

        int maxD = 0;
        int d = 0;
        for (const auto& e: data_) {
            if (e.second) {
                ++d;
            } else {
                --d;
            }
            maxD = max(maxD, d);
        }

        return maxD;
    }
};

/**
 * Your MyCalendarThree object will be instantiated and called as such:
 * MyCalendarThree* obj = new MyCalendarThree();
 * int param_1 = obj->book(start,end);
 */

int main() {
    MyCalendarThree sol;
    Timer t("Subarray timer");
    cerr << sol.book(10, 20) << endl << sol.book(50, 60) << endl << sol.book(10, 40) << endl <<sol.book(5, 15) << endl << sol.book(5, 10) << endl << sol.book(25, 55) << endl;

    return 0;
}
