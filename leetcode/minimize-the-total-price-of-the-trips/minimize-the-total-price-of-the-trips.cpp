#include "../header.h"

class Solution {
public:

    // helper function to find one end of the diameter using BFS
    int bfs(int start, vector<vector<int>>& adj) {
        queue<int> q;
        vector<bool> visited(adj.size(), false);
        q.push(start);
        visited[start] = true;
        int last = start; // store the last node visited
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            last = u; // update the last node visited
            for (int v : adj[u]) {
                if (!visited[v]) {
                    q.push(v);
                    visited[v] = true;
                }
            }
        }

        return last;
    }

    // helper function to find the minimum price sum of a trip using BFS
    int bfs(int start, int end, vector<vector<int>>& adj, vector<int>& price, unordered_map<int, int>& counts) {
        queue<int> q;
        vector<bool> visited(adj.size(), false);
        vector<int> cost(adj.size(), 0); // store the cost from start to each node
        vector<int> parent(adj.size(), -1); // store the cost from start to each node
        q.push(start);
        visited[start] = true;
        cost[start] = price[start];
        parent[start] = -1;

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            if (u == end) break; // found the destination node
            for (int v : adj[u]) {
                if (!visited[v]) {
                    q.push(v);
                    visited[v] = true;
                    parent[v] = u;
                    cost[v] = cost[u] + price[v]; // update the cost from start to v
                }
            }
        }

        int v = end;
        do {
            counts[v] += price[v];
            v = parent[v];
        } while (v != -1);

        return cost[end];
    }

    int bt(int n, const vector<vector<int>>& adj, int now, vector<bool>& bad, unordered_map<int, int>& counts,
           int sum) {
        if (now >= n) {
            return sum;
        }

        bool good = counts.count(now) && !bad[now];

        int baseline = min(sum, bt(n, adj, now + 1, bad, counts, sum));
        if (good) {
            auto nbad = bad;
            for (int u: adj[now]) {
                nbad[u] = true;
            }

            int hv = min(sum, bt(n, adj, now + 1, nbad, counts, sum - counts[now] / 2));

            return min({baseline, sum, hv});
        } else {
            return baseline;
        }
    }

    int maxIndependentSet(vector<vector<int>>& adj, unordered_map<int, int>& counts) {
        const int n = adj.size();

        // use dynamic programming to store the maximum independent set size for each node
        // dp[i][0] = maximum independent set size if node i is not included
        // dp[i][1] = maximum independent set size if node i is included
        vector<vector<int>> dp(n, vector<int>(2, 0));

        // use a helper function to fill the dp table using post-order traversal
        vector<bool> visited(n, false);
        dfs(0, adj, dp, visited, counts);  // arbitrary root node

        // return the maximum of dp[0][0] and dp[0][1]
        return max(dp[0][0], dp[0][1]);
    }

    // helper function to fill the dp table using post-order traversal
    void dfs(int u, vector<vector<int>>& adj, vector<vector<int>>& dp, vector<bool>& visited,
             unordered_map<int, int>& counts) {
        visited[u] = true;

        // loop through the children of u
        for (int v : adj[u]) {
            if (!visited[v]) {
                dfs(v, adj, dp, visited, counts);  // recursively fill the dp table for v

                // update the dp table for u
                dp[u][0] += max(dp[v][0], dp[v][1]);  // if u is not included, it can take either v or not
                dp[u][1] += dp[v][0];                 // if u is included, it cannot take v
            }
        }

        // add 1 to dp[u][1] as u is included
        dp[u][1] += counts[u] / 2;
    }

 int minimumTotalPrice(int n, const vector<vector<int>>& edges, const vector<int>& price_,
                          const vector<vector<int>>& trips) {
        auto price = price_;

        // build the adjacency list of the tree
        vector<vector<int>> adj(n);
        for (auto& edge : edges) {
            adj[edge[0]].push_back(edge[1]);
            adj[edge[1]].push_back(edge[0]);
        }

        // halve the prices of the non-adjacent nodes on the diameter
        unordered_map<int, int> counts;

        int ans0 = 0;
        for (auto& trip : trips) {
            ans0 += bfs(trip[0], trip[1], adj, price, counts);
        }

        int ans2 = ans0;

        if (false)
        {
            vector<bool> halved(n, false);
            ans2 = bt(n, adj, 0, halved, counts, ans0);
        }

        {
            ans2 = ans0 - maxIndependentSet(adj, counts);
        }

        vector<pair<int, int>> best;
        for (const auto& kv: counts) {
            best.emplace_back(make_pair(-kv.second, kv.first));
        }
        sort(best.begin(), best.end());
        // cerr << best << endl;

        unordered_set<int> halved;
        for (const auto& kv: best) {
            int cand = kv.second;
            bool good = true;
            for (int u : adj[cand]) {
                if (halved.count(u)) {
                    good = false;
                }
            }
            if (good) {
                halved.emplace(cand);
                price[cand] /= 2;
            }
        }

        // cerr << vector<int>(halved.begin(), halved.end()) << endl;
        // cerr << price << endl;

        int ans = 0;
        for (auto& trip : trips) {
            int d = bfs(trip[0], trip[1], adj, price, counts);
            // cerr << trip << ": " << d << endl;
            ans += d;
        }

        return min({ans, ans2, ans0});
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minimumTotalPrice(4, {{0, 1}, {1, 2}, {1, 3}}, {2, 2, 10, 6}, {{0, 3}, {2, 1}, {2, 3}}) << endl;
    cerr << sol.minimumTotalPrice(2, {{0, 1}}, {2, 2}, {{0, 0}}) << endl;
    // 826
    cerr << sol.minimumTotalPrice(
                10, {{0, 4}, {2, 4}, {6, 3}, {8, 3}, {3, 4}, {4, 7}, {7, 5}, {5, 1}, {1, 9}},
                {10, 6, 4, 8, 8, 6, 10, 8, 2, 2},
                {{9, 6}, {5, 5}, {8, 4}, {7, 8}, {4, 5}, {0, 8}, {3, 4}, {6, 1}, {8, 0}, {4, 5}, {7, 5},
                 {9, 0}, {6, 3}, {0, 1}, {3, 9}, {6, 7}, {1, 5}, {0, 9}, {0, 4}, {2, 0}, {0, 3}, {1, 8},
                 {5, 3}, {6, 0}, {6, 4}, {9, 0}, {8, 7}, {5, 6}, {3, 6}, {8, 8}, {7, 8}, {6, 3}, {5, 7},
                 {5, 3}, {8, 7}, {7, 7}, {2, 5}, {4, 2}, {0, 8}, {3, 2}, {7, 2}, {1, 6}, {2, 7}, {1, 7}})
         << endl;
    // 429
    cerr << sol.minimumTotalPrice(9, {{2, 5}, {3, 4}, {4, 1}, {1, 7}, {6, 7}, {7, 0}, {0, 5}, {5, 8}},
                                  {4, 4, 6, 4, 2, 4, 2, 14, 8},
                                  {{1, 5}, {2, 7}, {4, 3}, {1, 8}, {2, 8}, {4, 3}, {1, 5}, {1, 4}, {2, 1}, {6, 0},
                                   {0, 7}, {8, 6}, {4, 0}, {7, 5}, {7, 5}, {6, 0}, {5, 1}, {1, 1}, {7, 5}, {1, 7},
                                   {8, 7}, {2, 3}, {4, 1}, {3, 5}, {2, 5}, {3, 7}, {0, 1}, {5, 8}, {5, 3}, {5, 2}})
         << endl;

    cerr << sol.minimumTotalPrice(
                39, {{0, 24},  {1, 2},   {2, 27},  {10, 23}, {17, 27}, {19, 12}, {12, 37}, {20, 30}, {22, 9},  {9, 33},
                     {23, 21}, {25, 32}, {26, 30}, {27, 35}, {28, 18}, {29, 18}, {18, 4},  {32, 21}, {33, 24}, {24, 21},
                     {21, 38}, {34, 15}, {15, 3},  {3, 16},  {16, 31}, {31, 8},  {8, 6},   {6, 4},   {4, 14},  {35, 7},
                     {36, 5},  {5, 14},  {14, 13}, {13, 30}, {30, 11}, {37, 7},  {7, 11},  {11, 38}},
                {34, 34, 48, 34, 16, 54, 2,  34, 56, 20, 42, 40, 46, 62, 46, 18, 20, 6,  2, 6,
                 44, 50, 14, 12, 6,  38, 26, 40, 8,  30, 2,  28, 44, 60, 62, 16, 40, 16, 2},
                {{7, 32},  {18, 34}, {34, 10}, {1, 11},  {27, 4},  {27, 26}, {33, 2},  {15, 4},  {3, 10},  {34, 11},
                 {16, 32}, {6, 21},  {0, 11},  {33, 8},  {15, 37}, {9, 35},  {17, 4},  {32, 5},  {13, 34}, {17, 16},
                 {27, 38}, {29, 11}, {32, 6},  {23, 7},  {18, 26}, {12, 35}, {7, 35},  {31, 37}, {5, 20},  {18, 14},
                 {22, 29}, {16, 25}, {34, 23}, {24, 26}, {10, 31}, {30, 17}, {33, 13}, {27, 3},  {9, 31},  {12, 24},
                 {35, 35}, {10, 2},  {7, 30},  {7, 0},   {30, 2},  {4, 12},  {10, 14}, {13, 23}, {8, 22},  {12, 21},
                 {37, 38}, {17, 1},  {27, 14}, {27, 26}, {7, 14},  {21, 31}, {4, 16},  {3, 3},   {12, 32}, {38, 22},
                 {32, 33}, {25, 5},  {5, 35},  {2, 0},   {22, 14}, {35, 34}, {38, 9},  {9, 32},  {37, 11}, {3, 17},
                 {13, 24}, {9, 3},   {25, 29}, {28, 0},  {28, 31}, {5, 29},  {37, 5},  {9, 26},  {0, 5},   {29, 17},
                 {8, 7},   {23, 2},  {27, 5},  {16, 21}, {31, 35}, {38, 24}, {0, 21},  {3, 14},  {10, 31}, {20, 35},
                 {25, 30}, {9, 38},  {6, 20},  {29, 34}, {27, 21}, {19, 8},  {36, 21}, {33, 36}, {31, 37}, {16, 33}})
         << endl;

    return 0;
}
