#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLinesAll();
    unordered_map<string, vector<string>> g;

    for (const string& s: input) {
        if (split(s, ' ').size() > 3) {
            auto a = s.substr(0, 3);
            auto b = s.substr(7, 3);
            auto c = s.substr(12, 3);
            g[a].emplace_back(b);
            g[a].emplace_back(c);
        }
    }

    string now = "AAA";
    int len = 0;
    while (now != "ZZZ") {
        if (input[0][len % input[0].size()] == 'L') {
            now = g[now][0];
        } else {
            now = g[now][1];
        }
        ++len;
    }
    cout << OUT(len) << endl;

    unordered_map<string, int> dp;

    queue<string> q;
    dp["AAA"] = 0;
    q.emplace("AAA");

    while (dp.count("ZZZ") == 0) {
        string now = q.front();
        q.pop();
        for (int i = 0; i < 2; ++i) {
            if (dp.count(g[now][i]) == 0) {
                dp[g[now][i]] = dp[now] + 1;
                q.emplace(g[now][i]);
            }
        }
    }

    int result = dp["ZZZ"];
    cout << result << endl;
}

void second() {
    const auto input = readInputLinesAll();
    unordered_map<string, vector<string>> g;

    for (const string& s: input) {
        if (split(s, ' ').size() > 3) {
            auto a = s.substr(0, 3);
            auto b = s.substr(7, 3);
            auto c = s.substr(12, 3);
            g[a].emplace_back(b);
            g[a].emplace_back(c);
        }
    }

    set<string> states;
    for (const auto& kv: g) {
        if (kv.first.back() == 'A') {
            states.emplace(kv.first);
        }
    }

    for (const string& s0: states) {
        string now = s0;
        int len = 0;
        while (now.back() != 'Z') {
            if (input[0][len % input[0].size()] == 'L') {
                now = g[now][0];
            } else {
                now = g[now][1];
            }
            ++len;
        }
        cout << OUT(s0) << OUT(len) << endl;
    }

    auto add = [&](set<string>& st, const string& s) { st.emplace(s); };
    auto allZ = [&](const set<string>& s) {
        for (const auto& ss: s) {
            if (ss.back() != 'Z') {
                return false;
            }
        }
        return true;
    };

    int len = 0;
    while (!allZ(states)) {
        LOG_EVERY_MS(INFO, 1000) << OUT(len) << OUT((vector<string>{states.begin(), states.end()}));
        set<string> newStates;
        for (const string& s: states) {
            if (input[0][len % input[0].size()] == 'L') {
                add(newStates, g[s][0]);
            } else {
                add(newStates, g[s][1]);
            }
        }
        ++len;
        swap(states, newStates);
    }
    cout << "!!" << OUT(len) << endl;

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

