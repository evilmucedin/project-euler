#include <glog/logging.h>

#include "lib/header.h"

int main() {
    static constexpr int kN = 10000;
    vector<IntVector> graph(kN);
    for (int i = 0; i < kN; ++i) {
        int last3 = i % 1000;
        for (int j = 0; j < 10; ++j) {
            graph[i].push_back(10*last3 + j);
        }
    }
    BoolVector used(kN);
    IntVector stack(1, 1234);
    used[stack.back()] = true;
    int next = -1;
    while (stack.size() != kN && !stack.empty()) {
        LOG_EVERY_MS(INFO, 1000) << stack.size();
        int last = stack.back();
        int iV = 0;
        while (iV < graph[last].size() && (used[graph[last][iV]] || graph[last][iV] <= next)) {
            ++iV;
        }
        if (iV == graph[last].size()) {
            next = last;
            used[last] = false;
            stack.pop_back();
        } else {
            next = -1;
            int v = graph[last][iV];
            used[v] = true;
            stack.push_back(v);
        }
    }
    cout << stack << endl;
    return 0;
}
