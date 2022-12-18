#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

struct Vertex {
    string name_;
    int rate_;
    vector<string> n_;
};
using Graph = unordered_map<string, Vertex>;

void rec(const Graph& g, int t, int rateSoFar, int flow, const string& now, int& maxResult,
         unordered_map<string, int>& visited, unordered_set<string>& opened, int maxOpened) {
    LOG_EVERY_MS(INFO, 1000) << t << " " << rateSoFar << " " << flow << " " << OUT(maxResult) << OUT(visited.size())
                             << OUT(opened.size());

    if (t > 30) {
        maxResult = max(flow, maxResult);
        return;
    }

    if (opened.size() == maxOpened) {
        rec(g, 31, rateSoFar, flow + (31 - t) * rateSoFar, now, maxResult, visited, opened, maxOpened);
        return;
    }

    flow += rateSoFar;

    auto toNow = g.find(now);

    if (!opened.count(now) && toNow->second.rate_) {
        opened.emplace(now);
        rec(g, t + 1, rateSoFar + toNow->second.rate_, flow, now, maxResult, visited, opened, maxOpened);
        opened.erase(now);
    }

    rec(g, t + 1, rateSoFar, flow, now, maxResult, visited, opened, maxOpened);

    for (const string& n: toNow->second.n_) {
        ++visited[n];
        rec(g, t + 1, rateSoFar, flow, n, maxResult, visited, opened, maxOpened);
        --visited[n];
    }
}

void first() {
    const auto input = readInputLines();
    Graph g;
    int maxOpened = 0;
    for (const string& s: input) {
        Vertex v;
        const auto parts = split(s, ' ');
        const auto ints = parseIntegers(s);
        v.name_ = parts[1];
        v.rate_ = ints[0];
        if (v.rate_) {
            ++maxOpened;
        }
        for (int i = 9; i < parts.size(); ++i) {
            v.n_.emplace_back(replaceAll(parts[i], ",", ""));
        }
        g[v.name_] = v;
    }
    cerr << OUT(maxOpened) << endl;

    int result = 0;

    unordered_map<string, int> visited;
    unordered_set<string> opened;
    rec(g, 0, 0, 0, "AA", result, visited, opened, maxOpened);

    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}

