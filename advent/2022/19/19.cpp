#include <set>

#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

struct Cost {
    int oreRobotOre_;
    int clayRobotOre_;
    int obsidianRobotOre_;
    int obsidianRobotClay_;
    int geodeRobotOre_;
    int geodeRobotObsidian_;
};

Cost readCost(const string& input) {
    const auto parts = parseIntegers(input);
    cerr << "parts: " << parts << endl;
    return Cost{parts[0], parts[1], parts[2], parts[3], parts[4], parts[5]};
}

struct State {
    int t_;
    int oreRobot_;
    int ore_;
    int clayRobot_;
    int clay_;
    int obsidianRobot_;
    int obsidian_;
    int geodeRobot_;
    int geode_;

    tuple<int, int, int, int, int, int, int, int, int> toTuple() const {
        return tie(t_, oreRobot_, ore_, clayRobot_, clay_, obsidianRobot_, obsidian_, geodeRobot_, geode_);
    }

    bool operator<(const State& rhs) const {
        return toTuple() < rhs.toTuple();
    }
};
using States = set<State>;

int mg = 0;

void bt(const Cost& cost, const State& s, States& states) {
    if (states.count(s)) {
        return;
    }

    states.emplace(s);

    if (s.t_ == 4) {
        // cerr << "? " << OUT(s.toTuple()) << OUT(mg) << endl;
    }

    if (s.t_ == 24) {
        if (s.geode_ > mg) {
            mg = s.geode_;
            // cerr << "? " << OUT(s.toTuple()) << OUT(mg) << endl;
        }
        return;
    }

    LOG_EVERY_MS(INFO, 5000) << OUT(s.toTuple()) << OUT(mg) << endl;

    State s1 = s;
    ++s1.t_;

    {
        State s3 = s1;
        while (s3.ore_ >= cost.geodeRobotOre_ && s3.obsidian_ >= cost.geodeRobotObsidian_) {
            s3.ore_ -= cost.geodeRobotOre_;
            s3.obsidian_ -= cost.geodeRobotObsidian_;
            ++s3.geodeRobot_;
        }
        while (s3.ore_ >= cost.obsidianRobotOre_ && s3.clay_ >= cost.obsidianRobotClay_) {
            s3.ore_ -= cost.obsidianRobotOre_;
            s3.clay_ -= cost.obsidianRobotClay_;
            ++s3.obsidianRobot_;
        }
        while (s3.ore_ >= cost.clayRobotOre_) {
            s3.ore_ -= cost.clayRobotOre_;
            ++s3.clayRobot_;
        }
        while (s3.ore_ >= cost.oreRobotOre_) {
            s3.ore_ -= cost.oreRobotOre_;
            ++s3.oreRobot_;
        }
        s3.ore_ += s1.oreRobot_;
        s3.clay_ += s1.clayRobot_;
        s3.obsidian_ += s1.obsidianRobot_;
        s3.geode_ += s1.geodeRobot_;
        bt(cost, s3, states);
    }
    {
        s1.ore_ += s.oreRobot_;
        s1.clay_ += s.clayRobot_;
        s1.obsidian_ += s.obsidianRobot_;
        s1.geode_ += s.geodeRobot_;

        bt(cost, s1, states);
    }


    /*
    {
        State s2 = s1;
        while (s2.ore_ >= cost.geodeRobotOre_ && s2.obsidian_ >= cost.geodeRobotObsidian_) {
            s2.ore_ -= cost.geodeRobotOre_;
            s2.obsidian_ -= cost.geodeRobotObsidian_;
            ++s2.geodeRobot_;
            bt(cost, s2, states);
        }
    }
    {
        State s2 = s1;
        while (s2.ore_ >= cost.obsidianRobotOre_ && s2.clay_ >= cost.obsidianRobotClay_) {
            s2.ore_ -= cost.obsidianRobotOre_;
            s2.clay_ -= cost.obsidianRobotClay_;
            ++s2.obsidianRobot_;
            bt(cost, s2, states);
        }
    }
    {
        State s2 = s1;
        while (s2.ore_ >= cost.clayRobotOre_) {
            s2.ore_ -= cost.clayRobotOre_;
            ++s2.clayRobot_;
            bt(cost, s2, states);
        }
    }
    {
        State s2 = s1;
        while (s2.ore_ >= cost.oreRobotOre_) {
            s2.ore_ -= cost.oreRobotOre_;
            ++s2.oreRobot_;
            bt(cost, s2, states);
        }
    }
    */
}

void first() {
    const auto input = readInputLines();
    int result = 0;

    int index = 0;
    for (const string& in : input) {
        Cost cost = readCost(in);
        State s0 = {0, 1, 0, 0, 0, 0, 0, 0, 0};
        States states;
        mg = 0;
        bt(cost, s0, states);

        ++index;
        cerr << "! " << index << " " << mg << endl;
        result += mg*index;
    }

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

