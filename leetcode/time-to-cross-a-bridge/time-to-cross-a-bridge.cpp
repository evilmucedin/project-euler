#include "../header.h"

class Solution {
public:
    int findCrossingTime(int n, int k, const vector<vector<int>>& time) {
        struct Worker {
            int id;
            int leftToRight;
            int pickOld;
            int rightToLeft;
            int putNew;
            int state{};
            int remains{};
            int efficiency{};
            int lastLeft{};

            bool operator<(const Worker& a) {
                if (efficiency != a.efficiency) {
                    return efficiency > a.efficiency;
                }
                return id < a.id;
            }
        };

        int ans = 0;
        int boxes = n;
        int workersNeed = n;
        vector<Worker> w(k);
        for (int i = 0; i < k; ++i) {
            w[i].id = i;
            w[i].leftToRight = time[i][0];
            w[i].pickOld = time[i][1];
            w[i].rightToLeft = time[i][2];
            w[i].putNew = time[i][3];
            w[i].state = 0;
            w[i].remains = 0;
            w[i].efficiency = w[i].leftToRight + w[i].rightToLeft;
        }

        while (true) {
            vector<int> counts = {0, 0, 0, 0, 0, 0};
            bool hasRemains = false;
            for (int i = 0; i < k; ++i) {
                if (w[i].remains > 0) {
                    --w[i].remains;
                    if (w[i].remains) {
                        hasRemains = true;
                    }
                }
                if (w[i].remains == 0) {
                    if (w[i].state == 1) {
                        w[i].state = 2;
                        w[i].remains = w[i].pickOld;
                    } else if (w[i].state == 2) {
                        w[i].state = 3;
                    } else if (w[i].state == 4) {
                        w[i].state = 5;
                        w[i].remains = w[i].putNew;
                        w[i].lastLeft = ans;
                    } else if (w[i].state == 5) {
                        w[i].state = 0;
                        --boxes;
                    }
                }
                ++counts[w[i].state];
            }

            cerr << ans << " " << counts << endl;

            if (counts[0] == k && boxes == 0 && !hasRemains) {
                break;
            }

            auto lowest = [&](int state) {
                int result = -1;
                for (int i = 0; i < k; ++i) {
                    if (w[i].state == state) {
                        if (result == -1 || w[i] < w[result]) {
                            result = i;
                        }
                    }
                }
                return result;
            };

            if (counts[1] == 0 && counts[4] == 0) {
                if (counts[3] > 0) {
                    int workerId = lowest(3);
                    w[workerId].state = 4;
                    w[workerId].remains = w[workerId].rightToLeft;
                } else if (counts[0] > 0 && workersNeed > 0) {
                    --workersNeed;
                    int workerId = lowest(0);
                    w[workerId].state = 1;
                    w[workerId].remains = w[workerId].leftToRight;
                }
            }

            ++ans;
        }

        ans = 0;
        for (const auto& wk: w) {
            ans = max(ans, wk.lastLeft);
        }

        return ans;
    }
};

/*

struct Node {
  int id;
  int time;
  int efficiency;
};

struct Cmp {
  bool operator()(Node& a, Node& b) {
    if (a.efficiency != b.efficiency) {
      return a.efficiency > b.efficiency;
    }
    return a.id > b.id;
  }
};

class Solution {
public:
    int findCrossingTime(int n, int k, vector<vector<int>>& time) {
        priority_queue<Node, std::vector<Node>, Cmp> left;
        priority_queue<Node, std::vector<Node>, Cmp> right;
        for (int i = 0; i < k; i++) {
            left.push({i, 0, time[i][0] + time[i][2]});
        }
        int ans = 0;
        int boxes = n;
        while (boxes > 0) {
            Node worker = left.top();
            left.pop();
            ans = std::max(ans, worker.time + time[worker.id][0]);
            worker.time = ans + time[worker.id][1];
            right.push(worker);
            while (!right.empty() && boxes > 0) {
                worker = right.top();
                right.pop();
                ans = std::max(ans, worker.time + time[worker.id][2]);
                worker.time = ans + time[worker.id][3];
                boxes--;
                left.push(worker);
            }
        }
        // ans = 0;
        while (!right.empty()) {
            Node worker = right.top();
            right.pop();
            ans = std::max(ans, worker.time);
        }
        while (!left.empty()) {
            Node worker = left.top();
            left.pop();
            ans = std::max(ans, worker.time);
        }
        return ans;
    }
};

*/

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findCrossingTime(1, 3, {{1, 1, 2, 1}, {1, 1, 3, 1}, {1, 1, 4, 1}}) << endl;
    cerr << sol.findCrossingTime(3, 2, {{1, 9, 1, 8}, {10, 10, 10, 10}}) << endl;

    return 0;
}
