#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

struct Item {
    vector<vector<int>> rooms;
    vector<int> hallway = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    void print() const {
        REP(i, 4) {
            if (rooms[i].size() == 2) {
                cerr << char(rooms[i][0]) << ", " << char(rooms[i][1]) << endl;
            } else if (rooms[i].size() == 1) {
                cerr << char(rooms[i][0]) << endl;
            } else {
                cerr << endl;
            }
        }
        REP(j, 11) {
            cerr << (hallway[j] ? char(hallway[j]) : '.') << " ";
        }
        cerr << endl;
    }

    bool operator<(const Item& rhs) const {
        if (rooms != rhs.rooms) {
            return rooms < rhs.rooms;
        }
        return hallway < rhs.hallway;
    }
};

Item read() {
    const auto input = readInputLines();
    Item res;
    res.rooms.resize(4);
    REP (i, 4) {
        res.rooms[i].resize(2);
        REP (j, 2) {
            res.rooms[i][j] = input[3 - j][3 + 2*i];
        }
    }
    return res;
}

bool isFinal(const Item& f) {
    for (auto h: f.hallway) {
        if (h) {
            return false;
        }
    }
    REP (i, 4) {
        if (f.rooms[i].size() != 2) {
            return false;
        }
        REP (j, 2) {
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
    int x = room*2 + 3;
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
    int x = room*2 + 3;
    if (x == hallway) {
        return true;
    }
    if (x < hallway) {
        for (int i = x; i < hallway; ++i) {
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

int cost1(int roomSize, int room, int hallway) {
    hallway += 1;
    int x = room*2 + 3;
    int res = abs(x - hallway);
    if (roomSize == 2) {
        ++res;
    } else if (roomSize == 1) {
        res += 2;
    } else if (roomSize == 0) {
        res += 3;
    }
    return res;
}

int cost2(int roomSize, int room, int hallway) {
    hallway += 1;
    int x = room*2 + 3;
    int res = abs(x - hallway);
    if (roomSize == 2) {
    } else if (roomSize == 1) {
        res += 1;
    } else if (roomSize == 0) {
        res += 2;
    }
    return res;
}

bool goodRoom(const Item& f, int room) {
    for (int i = 0; i < f.rooms[room].size(); ++i) {
        if (f.rooms[room][i] != room + 'A') {
            return false;
        }
    }
    return true;
}

int best = 1000000000;

priority_queue<pair<int, Item>> q;
map<Item, int> bestD;

void dfs(const Item& f0) {
    q.emplace(0, f0);

    while (!q.empty()) {
        auto tp = q.top();
        q.pop();
        const auto f = tp.second;
        const int d = -tp.first;

        LOG_EVERY_MS(INFO, 1000) << d << " " << best;

        if (isFinal(f)) {
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
                        auto nd = d + cost1(nf.rooms[i].size(), i, j) * costA(a);
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
                        auto nd = d + cost2(nf.rooms[i].size(), i, j) * costA(a);
                        nf.rooms[i].push_back(a);
                        nf.hallway[j] = 0;
                        q.emplace(-nd, nf);
                    }
                }
            }
        }
    }
}

void first() {
    const auto input = read();

    input.print();

    dfs(input);

    cout << best << endl;
}

void second() {
    const auto input = readInputLines();
    cout << endl;
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

