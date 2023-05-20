#include "../header.h"

class Solution {
public:
    /*
    void dfs(int node, vector<int>& cost, int curr_cost, vector<int>& min_cost, vector<int>& max_cost) {
        if (2 * node > cost.size()) {
            // leaf node
            min_cost[node] = curr_cost;
            max_cost[node] = curr_cost;
            return;
        }
        dfs(2 * node, cost, curr_cost + cost[node - 1], min_cost, max_cost);
        dfs(2 * node + 1, cost, curr_cost + cost[node - 1], min_cost, max_cost);
        min_cost[node] = min(min_cost[2 * node], min_cost[2 * node + 1]);
        max_cost[node] = max(max_cost[2 * node], max_cost[2 * node + 1]);
    }

    int minIncrements(int n, vector<int>& cost) {
        vector<int> min_cost(n + 1), max_cost(n + 1);
        dfs(1, cost, 0, min_cost, max_cost);
        int diff = max_cost[1] - min_cost[1];
        return diff;
    }
    */


    int minIncrements(int n, const vector<int>& cost) {
        vector<int> fullcost(n);
        vector<int> mincost(n);
        vector<int> maxcost(n);
        vector<int> inccost(n);

        dfs__(n, 0, 0, cost, fullcost, mincost, maxcost);

        cerr << OUT(fullcost) << endl;
        cerr << OUT(mincost) << endl;
        cerr << OUT(maxcost) << endl;

        int mx = 0;
        int m = (n + 1) / 2;
        for (int i = n - m; i < n; ++i) {
            mx = max(mx, fullcost[i]);
        }

        dfs(n, 0, 0, cost, mx, inccost, mincost, maxcost, 0);

        cerr << OUT(inccost) << endl;

        int result = 0;
        for (int i = 0; i < n; ++i) {
            result += inccost[i];
        }
        return result;
    }

    void dfs(int n, int now, int nowCost, const vector<int>& cost, int target, vector<int>& inccost, vector<int>& mincost, vector<int>& maxcost, int total) {
        if (now >= n) {
            return;
        }

        nowCost += cost[now];
        if (now*2 + 1 >= n) {
            assert(nowCost <= target);
            inccost[now] = target - nowCost;
        } else {
            inccost[now] = target - maxcost[now] - total;
            inccost[now] = max(inccost[now], 0);
            dfs(n, now*2 + 1, nowCost + inccost[now], cost, target, inccost, mincost, maxcost, total + inccost[now]);
            dfs(n, now*2 + 2, nowCost + inccost[now], cost, target, inccost, mincost, maxcost, total + inccost[now]);
        }
    }

    void dfs__(int n, int now, int nowCost, const vector<int>& cost, vector<int>& fullcost, vector<int>& mincost, vector<int>& maxcost) {
        if (now >= n) {
            return;
        }
        nowCost += cost[now];
        fullcost[now] = nowCost;

        dfs__(n, now*2 + 1, nowCost, cost, fullcost, mincost, maxcost);
        dfs__(n, now*2 + 2, nowCost, cost, fullcost, mincost, maxcost);

        if (now*2 + 1 >= n) {
            mincost[now] = fullcost[now];
            maxcost[now] = fullcost[now];
        } else {
            mincost[now] = min(mincost[now*2 + 1], mincost[now*2 + 2]);
            maxcost[now] = max(maxcost[now*2 + 1], maxcost[now*2 + 2]);
        }
    }
};


int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minIncrements(7, {1, 5, 2, 2, 3, 3, 1}) << endl;
    cerr << sol.minIncrements(15,
                              {764, 1460, 2664, 764, 2725, 4556, 5305, 8829, 5064, 5929, 7660, 6321, 4830, 7055, 3761})
         << endl;

    return 0;
}
