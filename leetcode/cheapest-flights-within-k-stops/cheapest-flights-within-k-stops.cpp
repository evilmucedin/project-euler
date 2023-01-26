#include "../header.h"

class Solution {
   public:
    int findCheapestPrice(int n, const vector<vector<int>>& flights, int src, int dst, int k) {
        struct Edge : public pair<int, int> {
            using pair::pair;

            int& dst() { return first; }

            int& cost() { return second; }

            const int& dst() const { return first; }

            const int& cost() const { return second; }
        };

        vector<vector<Edge>> adj(n);

        for (const auto& f : flights) {
            adj[f[0]].emplace_back(Edge{f[1], f[2]});
        }

        static constexpr int INF = 1000000000;
        vector<int> d(n, INF);
        vector<int> len(n, INF);
        d[src] = 0;
        len[src] = 0;
        queue<pair<int, int>> q;
        q.emplace(make_pair(src, 0));

        for (int l = 0; l <= k; ++l) {
            const int sz = q.size();
            // cerr << "\t" << l << ": " << sz << endl;

            for (int i = 0; i < sz; ++i) {
                const auto tp = q.front();
                q.pop();
                // cerr << "\t\t" << tp.first << " "
                //      << " " << d[tp.first] << " " << len[tp.first] << endl;
                for (const auto& e : adj[tp.first]) {
                    int cost = tp.second + e.cost();
                    int ln = len[tp.first] + 1;
                    if (d[e.dst()] > cost) {
                        q.push(make_pair(e.dst(), cost));
                        d[e.dst()] = cost;
                        len[e.dst()] = ln;
                    }
                }
            }
        }

        // cerr << adj << endl;
        // cerr << d << endl;
        // cerr << len << endl;

        if (d[dst] != INF) {
            return d[dst];
        }
        return -1;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.findCheapestPrice(4, {{0, 1, 100}, {1, 2, 100}, {2, 0, 100}, {1, 3, 600}, {2, 3, 200}}, 0, 3, 1) << endl;
    cerr << sol.findCheapestPrice(4, {{0, 1, 1}, {0, 2, 5}, {1, 2, 1}, {2, 3, 1}}, 0, 3, 1) << endl;

    return 0;
}
