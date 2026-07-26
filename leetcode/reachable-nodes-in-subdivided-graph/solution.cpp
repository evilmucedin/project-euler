#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

using namespace std;

class Solution {
public:
    int reachableNodes(vector<vector<int>>& edges, int maxMoves, int n) {
        vector<vector<pair<int, int>>> graph(n);
        for (const auto& e : edges) {
            int u = e[0], v = e[1], cnt = e[2];
            graph[u].push_back({v, cnt + 1});
            graph[v].push_back({u, cnt + 1});
        }

        vector<int> dist(n, numeric_limits<int>::max());
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        dist[0] = 0;
        pq.push({0, 0});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d != dist[u]) {
                continue;
            }

            for (const auto& [v, w] : graph[u]) {
                if (d + w < dist[v]) {
                    dist[v] = d + w;
                    pq.push({dist[v], v});
                }
            }
        }

        int ans = 0;
        for (int i = 0; i < n; ++i) {
            if (dist[i] != numeric_limits<int>::max() && dist[i] <= maxMoves) {
                ++ans;
            }
        }

        for (const auto& e : edges) {
            int u = e[0], v = e[1], cnt = e[2];
            if (dist[u] == numeric_limits<int>::max() || dist[v] == numeric_limits<int>::max()) {
                continue;
            }

            int a = min(cnt, max(0, maxMoves - dist[u]));
            int b = min(cnt, max(0, maxMoves - dist[v]));
            ans += min(cnt, a + b);
        }

        return ans;
    }
};
