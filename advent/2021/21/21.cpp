#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "lib/init.h"
#include "lib/robinHood.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

int read(const string& s) {
    auto parts = split(s, ':');
    return stoi(trim(parts[1]));
}

void step(int& pos, int& points, int& rolls) {
    pos += (rolls % 100) + 1;
    pos += ((rolls + 1) % 100) + 1;
    pos += ((rolls + 2) % 100) + 1;
    rolls += 3;
    pos %= 10;
    points += pos + 1;
}

void first() {
    const auto input = readInputLines();

    int pos1 = read(input[0]);
    int pos2 = read(input[1]);

    // cerr << pos1 << " " << pos2 << endl;
    --pos1;
    --pos2;

    int points1 = 0;
    int points2 = 0;
    int turn = 0;
    int rolls = 0;

    // cerr << turn << " " << pos1 << " " << pos2 << endl;
    while (points1 < 1000 && points2 < 1000) {
        if (turn & 1) {
            step(pos2, points2, rolls);
        } else {
            step(pos1, points1, rolls);
        }
        ++turn;
        /*
        if (turn < 10) {
            cerr << turn << " {" << pos1 << " " << points1 << "} {" << pos2 << " " << points2 << "}" << endl;
        }
        */
    }

    cerr << points1 << " " << points2 << " " << rolls << endl;
    cout << "1: " <<  min(points1, points2) * rolls << endl;
}

using Res = pair<u64, u64>;
using Store = robin_hood::unordered_map<IntVector, Res>;

Store store;

void incRes(Res& res1, const Res& res2) {
    res1.first += res2.first;
    res1.second += res2.second;
}

Res countWins(int pos1, int points1, int pos2, int points2, int turn);

Res countWins_(int pos1, int points1, int pos2, int points2, int turn) {
    if (points1 >= 21) {
        return Res(1, 0);
    }
    if (points2 >= 21) {
        return Res(0, 1);
    }

    if (turn == 0) {
        Res res;
        for (size_t i1 = 1; i1 <= 3; ++i1) {
            for (size_t i2 = 1; i2 <= 3; ++i2) {
                for (size_t i3 = 1; i3 <= 3; ++i3) {
                    auto pos = pos1 + i1 + i2 + i3;
                    while (pos > 10) {
                        pos -= 10;
                    }
                    auto points = points1 + pos;
                    incRes(res, countWins(pos, points, pos2, points2, 1 - turn));
                }
            }
        }
        return res;
    } else {
        Res res;
        for (size_t i1 = 1; i1 <= 3; ++i1) {
            for (size_t i2 = 1; i2 <= 3; ++i2) {
                for (size_t i3 = 1; i3 <= 3; ++i3) {
                    auto pos = pos2 + i1 + i2 + i3;
                    while (pos > 10) {
                        pos -= 10;
                    }
                    auto points = points2 + pos;
                    incRes(res, countWins(pos1, points1, pos, points, 1 - turn));
                }
            }
        }
        return res;
    }
}

Res countWins(int pos1, int points1, int pos2, int points2, int turn) {
    const IntVector state = {pos1, points1, pos2, points2, turn};
    auto toState = store.find(state);
    if (toState == store.end()) {
        auto res = countWins_(pos1, points1, pos2, points2, turn);
        store.emplace(state, res);
        return res;
    }
    return toState->second;
}

void second() {
    const auto input = readInputLines();

    int pos1 = read(input[0]);
    int pos2 = read(input[1]);

    // cerr << pos1 << " " << pos2 << endl;

    const auto res = countWins(pos1, 0, pos2, 0, 0);
    cerr << res << endl;
    cout << "2: " << max(res.first, res.second) << endl;
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
