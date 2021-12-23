#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

struct Item {
    vector<IntVector> rooms;
    IntVector hallway = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    void print() const {
        REP(i, 4) {
            REP (j, rooms[i].size()) {
                if (j) {
                    cerr << ", ";
                }
                cerr << char(rooms[i][j]);
            }
            cerr << endl;
        }
        REP(j, 11) { cerr << (hallway[j] ? char(hallway[j]) : '.') << " "; }
        cerr << endl;
    }

    bool operator==(const Item& rhs) const { return rooms == rhs.rooms && hallway == rhs.hallway; }

    bool operator<(const Item& rhs) const {
        if (rooms != rhs.rooms) {
            return rooms < rhs.rooms;
        }
        return hallway < rhs.hallway;
    }
};

namespace std {
template <>
struct hash<Item> {
    size_t operator()(const Item& p) const {
        size_t result = 0;
        hashCombine(result, p.rooms, p.hallway);
        return result;
    }
};
}  // namespace std

Item read() {
    const auto input = readInputLines();
    Item res;
    res.rooms.resize(4);
    REP(i, 4) {
        res.rooms[i].resize(2);
        REP(j, 2) { res.rooms[i][j] = input[3 - j][3 + 2 * i]; }
    }
    return res;
}

template <int LEN>
bool isFinal(const Item& f) {
    for (auto h : f.hallway) {
        if (h) {
            return false;
        }
    }
    REP(i, 4) {
        if (f.rooms[i].size() != LEN) {
            return false;
        }
        REP(j, LEN) {
            if (f.rooms[i][j] != 'A' + i) {
                return false;
            }
        }
    }
    return true;
}

int costA(int a) {
    switch (a) {
        case 'A':
            return 1;
        case 'B':
            return 10;
        case 'C':
            return 100;
        case 'D':
            return 1000;
        default:
            throw Exception("costA");
    }
}

bool can1(const Item& f, int room, int hallway) {
    if (hallway == 2 || hallway == 4 || hallway == 6 || hallway == 8) {
        return false;
    }
    hallway += 1;
    const int x = room * 2 + 3;
    if (x < hallway) {
        for (int i = x; i <= hallway; ++i) {
            if (f.hallway[i - 1]) {
                return false;
            }
        }
    } else {
        for (int i = hallway; i <= x; ++i) {
            if (f.hallway[i - 1]) {
                return false;
            }
        }
    }
    return true;
}

bool can2(const Item& f, int room, int hallway) {
    if (hallway == 2 || hallway == 4 || hallway == 6 || hallway == 8) {
        return false;
    }
    hallway += 1;
    const int x = room * 2 + 3;
    if (x < hallway) {
        FOR (i, x, hallway) {
            if (f.hallway[i - 1]) {
                return false;
            }
        }
    } else {
        for (int i = hallway + 1; i <= x; ++i) {
            if (f.hallway[i - 1]) {
                return false;
            }
        }
    }
    return true;
}

template <int LEN>
int cost1(int roomSize, int room, int hallway) {
    int x = room * 2 + 3;
    return abs(x - hallway - 1) + LEN + 1 - roomSize;
}

template <int LEN>
int cost2(int roomSize, int room, int hallway) {
    int x = room * 2 + 3;
    return abs(x - hallway - 1) + LEN - roomSize;
}

bool goodRoom(const Item& f, int room) {
    for (auto a : f.rooms[room]) {
        if (a != room + 'A') {
            return false;
        }
    }
    return true;
}

template <int LEN>
int dijkstra(const Item& f0) {
    size_t its = 0;
    int best = 1000000000;

    priority_queue<pair<int, Item>> q;
    map<Item, int> bestD;

    q.emplace(0, f0);

    while (!q.empty()) {
        const auto tp = q.top();
        q.pop();
        const auto& f = tp.second;
        const int d = -tp.first;

        // LOG_EVERY_MS(INFO, 1000) << d << " " << best << " " << q.size();

        if (isFinal<LEN>(f)) {
            if (d < best) {
                best = d;
            }
            continue;
        }

        const auto toItem = bestD.find(f);
        if (toItem != bestD.end() && toItem->second <= d) {
            continue;
        }
        bestD.emplace(f, d);

        /*
        cerr << d << " " << best << endl;
        f.print();
        cerr << endl;
        */

        REP(i, 4) {
            if (!f.rooms[i].empty()) {
                REP(j, 11) {
                    if (can1(f, i, j)) {
                        auto nf = f;
                        const auto a = nf.rooms[i].back();
                        auto nd = d + cost1<LEN>(nf.rooms[i].size(), i, j) * costA(a);
                        nf.rooms[i].pop_back();
                        nf.hallway[j] = a;
                        q.emplace(-nd, nf);
                    }
                }
            }
        }

        REP(j, 11) {
            if (f.hallway[j]) {
                auto a = f.hallway[j];
                const int i = f.hallway[j] - 'A';
                if (goodRoom(f, i)) {
                    if (can2(f, i, j)) {
                        auto nf = f;
                        auto nd = d + cost2<LEN>(nf.rooms[i].size(), i, j) * costA(a);
                        nf.rooms[i].emplace_back(a);
                        nf.hallway[j] = 0;
                        q.emplace(-nd, nf);
                    }
                }
            }
        }

        ++its;
    }

    LOG(INFO) << OUT(its) << OUT(bestD.size());

    return best;
}

void first() {
    const auto input = read();
    // input.print();

    const auto res = dijkstra<2>(input);

    cout << "1: " << res << endl;
}

void second() {
    auto input = read();

    insertVector(input.rooms[0], 1, {'D', 'D'});
    insertVector(input.rooms[1], 1, {'B', 'C'});
    insertVector(input.rooms[2], 1, {'A', 'B'});
    insertVector(input.rooms[3], 1, {'C', 'A'});

    // input.print();

    const auto res = dijkstra<4>(input);

    cout << "2: " << res << endl;
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
