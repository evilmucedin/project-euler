#include "../header.h"

class Solution {
public:
    string countAndSay(int n) {
        string now = "1";
        for (int i = 1; i < n; ++i) {
            string next = "";
            int count = 0;
            char d = 0;
            for (int j = 0; j <= now.size(); ++j) {
                if (now[j] == d) {
                    ++count;
                } else {
                    if (d) {
                        next += to_string(count);
                        next += d;
                    }
                    d = now[j];
                    count = 1;
                }
            }
            now = next;
        }
        return now;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.countAndSay(4) << endl;

    return 0;
}
