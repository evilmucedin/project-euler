#include "../header.h"
#include "lib/unionFindDense.h"

class Solution {
public:
    /*
    int magnificentSets(int n, vector<vector<int>> edges) {
        static constexpr int INF = 1000000000;
        unordered_map<int, vector<int>> g;
        for (auto& v: edges) {
            --v[0];
            --v[1];
            g[v[0]].emplace_back(v[1]);
            g[v[1]].emplace_back(v[0]);
        }

        vector<vector<int>> d(n, vector<int>(n, INF));
        for (int i = 0; i < n; ++i) {
            d[i][i] = 0;
        }
        for (const auto& e: edges) {
            d[e[0]][e[1]] = 1;
            d[e[1]][e[0]] = 1;
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                for (int k = 0; k < n; ++k) {
                    d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
                }
            }
        }

        UnionFindDense uf(n);
        for (const auto& e: edges) {
            uf.unite(e[0], e[1]);
        }

        auto p2c = uf.parentToComponent();
        int result = 0;
        for (const auto& c: p2c) {
            int maxD = 0;
            for (auto v: c.second) {
                int maxDV = 0;
                for (auto u: c.second) {
                    maxDV = max(maxDV, d[u][v]);
                }
                maxD = max(maxD, maxDV);
            }
            result = maxD + 1;
        }
        return result;
    }
    */
    int magnificentSets(int n, vector<vector<int>> edges) {
        static constexpr int INF = 1000000000;
        unordered_map<int, vector<int>> g;
        for (auto& v: edges) {
            --v[0];
            --v[1];
            g[v[0]].emplace_back(v[1]);
            g[v[1]].emplace_back(v[0]);
        }

        UnionFindDense uf(n);
        for (const auto& e: edges) {
            uf.unite(e[0], e[1]);
        }

        auto p2c = uf.parentToComponent();

        vector<int> d(n, INF);
        int result = 0;
        for (int i = 0; i < n; ++i) {
            if (d[i] != INF) {
                continue;
            }
            int maxD = 0;
            for (int j : p2c[uf.find(i)]) {
                for (int k: p2c[uf.find(i)]) {
                    d[k] = INF;
                }

                d[j] = 0;
                queue<int> q;
                q.push(j);
                while (!q.empty()) {
                    int v = q.front();
                    q.pop();
                    for (auto u : g[v]) {
                        if (d[u] == INF) {
                            d[u] = d[v] + 1;
                            maxD = max(maxD, d[u]);
                            q.push(u);
                        }
                    }
                }
            }
            result += maxD + 1;
        }

        int j = 0;
        for (j = 0; j < edges.size(); ++j) {
            int a = edges[j][0];
            int b = edges[j][1];
            if (abs(d[a] - d[b]) != 1) {
                return -1;
            }
        }

        return result;
    }
    /*
    */
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    // 4
    cerr << sol.magnificentSets(6, {{1, 2}, {1, 4}, {1, 5}, {2, 6}, {2, 3}, {4, 6}}) << endl;
    // 30
    cerr << sol.magnificentSets(30, {{16, 8}, {6, 5}}) << endl;
    // 57
    cerr << sol.magnificentSets(
                92, {{67, 29}, {13, 29}, {77, 29}, {36, 29}, {82, 29}, {54, 29}, {57, 29}, {53, 29}, {68, 29}, {26, 29},
                     {21, 29}, {46, 29}, {41, 29}, {45, 29}, {56, 29}, {88, 29}, {2, 29},  {7, 29},  {5, 29},  {16, 29},
                     {37, 29}, {50, 29}, {79, 29}, {91, 29}, {48, 29}, {87, 29}, {25, 29}, {80, 29}, {71, 29}, {9, 29},
                     {78, 29}, {33, 29}, {4, 29},  {44, 29}, {72, 29}, {65, 29}, {61, 29}})
         << endl;
    // 4
    cerr << sol.magnificentSets(
                26, {{9, 16},  {8, 3},   {20, 21}, {12, 16}, {14, 3},  {7, 21},  {22, 3},  {22, 18}, {11, 16}, {25, 4},
                     {2, 4},   {14, 21}, {23, 3},  {17, 3},  {2, 16},  {24, 16}, {13, 4},  {10, 21}, {7, 4},   {9, 18},
                     {14, 18}, {14, 4},  {14, 16}, {1, 3},   {25, 18}, {17, 4},  {1, 16},  {23, 4},  {2, 21},  {5, 16},
                     {24, 18}, {20, 18}, {19, 16}, {24, 21}, {9, 3},   {24, 3},  {19, 18}, {25, 16}, {19, 21}, {6, 3},
                     {26, 18}, {5, 21},  {20, 16}, {2, 3},   {10, 18}, {26, 16}, {8, 4},   {11, 21}, {23, 16}, {13, 16},
                     {25, 3},  {7, 18},  {19, 3},  {20, 4},  {26, 3},  {23, 18}, {15, 18}, {17, 18}, {10, 16}, {26, 21},
                     {23, 21}, {7, 16},  {8, 18},  {10, 4},  {24, 4},  {7, 3},   {11, 18}, {9, 4},   {26, 4},  {13, 21},
                     {22, 16}, {22, 21}, {20, 3},  {6, 18},  {9, 21},  {10, 3},  {22, 4},  {1, 18},  {25, 21}, {11, 4},
                     {1, 21},  {15, 3},  {1, 4},   {15, 16}, {2, 18},  {13, 3},  {8, 21},  {13, 18}, {11, 3},  {15, 21},
                     {8, 16},  {17, 16}, {15, 4},  {12, 3},  {6, 4},   {17, 21}, {5, 18},  {6, 16},  {6, 21},  {12, 4},
                     {19, 4},  {5, 3},   {12, 21}, {5, 4}})
         << endl;

    return 0;
}
