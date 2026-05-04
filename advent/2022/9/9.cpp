#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

using Point = pair<int, int>;
using Points = vector<Point>;

bool nei(const Point& a, const Point& b) {
    if (make_pair(a.first + 1, a.second) == b) {
        return true;
    }
    if (make_pair(a.first - 1, a.second) == b) {
        return true;
    }
    if (make_pair(a.first, a.second + 1) == b) {
        return true;
    }
    if (make_pair(a.first, a.second - 1) == b) {
        return true;
    }
    if (make_pair(a.first + 1, a.second + 1) == b) {
        return true;
    }
    if (make_pair(a.first + 1, a.second - 1) == b) {
        return true;
    }
    if (make_pair(a.first - 1, a.second + 1) == b) {
        return true;
    }
    if (make_pair(a.first - 1, a.second - 1) == b) {
        return true;
    }
    return false;
}

void first() {
    const auto input = readInputLines();

    Point h(0, 0);

    Points hs;
    hs.emplace_back(h);
    for (const string& s : input) {
        const auto parts = split(s, ' ');
        int d = atoi(parts[1].c_str());
        for (int i = 0; i < d; ++i) {
            if (parts[0] == "R") {
                h.first += 1;
            } else if (parts[0] == "L") {
                h.first -= 1;
            } else if (parts[0] == "U") {
                h.second += 1;
            } else if (parts[0] == "D") {
                h.second -= 1;
            } else {
                THROW("Unknown dir");
            }
            // cerr << h << endl;
            hs.emplace_back(h);
        }
    }

    Point t(0, 0);
    unordered_set<pair<int, int>> tPos;
    tPos.emplace(t);
    for (const auto& hp: hs) {
        if (t.second == hp.second) {
            if (hp.first > t.first + 1) {
                ++t.first;
            } else if (hp.first < t.first - 1) {
                --t.first;
            }
        } else if (t.first == hp.first) {
            if (hp.second > t.second + 1) {
                ++t.second;
            } else if (hp.second < t.second - 1) {
                --t.second;
            }
        } else if (!nei(t, hp)) {
            if (t.second < hp.second && t.first < hp.first) {
                ++t.first;
                ++t.second;
            } else if (t.second > hp.second && t.first > hp.first) {
                --t.first;
                --t.second;
            } else if (t.second < hp.second && t.first > hp.first) {
                --t.first;
                ++t.second;
            } else if (t.second > hp.second && t.first < hp.first) {
                ++t.first;
                --t.second;
            }
        }
        // cerr << hp << " " << t << endl;
        tPos.emplace(t);
    }

    cout << tPos.size() << endl;
}

void second() {
    const auto input = readInputLines();

    Point h(0, 0);

    vector<Points> hs(10);
    hs[0].emplace_back(h);
    for (const string& s : input) {
        const auto parts = split(s, ' ');
        int d = atoi(parts[1].c_str());
        for (int i = 0; i < d; ++i) {
            if (parts[0] == "R") {
                h.first += 1;
            } else if (parts[0] == "L") {
                h.first -= 1;
            } else if (parts[0] == "U") {
                h.second += 1;
            } else if (parts[0] == "D") {
                h.second -= 1;
            } else {
                THROW("Unknown dir");
            }
            // cerr << h << endl;
            hs[0].emplace_back(h);
        }
    }

    for (int i = 1; i < 10; ++i) {

        Point t(0, 0);
        unordered_set<pair<int, int>> tPos;
        tPos.emplace(t);
        // hs[i].emplace_back(t);
        for (const auto& hp : hs[i - 1]) {
            if (t.second == hp.second) {
                if (hp.first > t.first + 1) {
                    ++t.first;
                } else if (hp.first < t.first - 1) {
                    --t.first;
                }
            } else if (t.first == hp.first) {
                if (hp.second > t.second + 1) {
                    ++t.second;
                } else if (hp.second < t.second - 1) {
                    --t.second;
                }
            } else if (!nei(t, hp)) {
                if (t.second < hp.second && t.first < hp.first) {
                    ++t.first;
                    ++t.second;
                } else if (t.second > hp.second && t.first > hp.first) {
                    --t.first;
                    --t.second;
                } else if (t.second < hp.second && t.first > hp.first) {
                    --t.first;
                    ++t.second;
                } else if (t.second > hp.second && t.first < hp.first) {
                    ++t.first;
                    --t.second;
                }
            }
            // cerr << hp << " " << t << endl;
            tPos.emplace(t);
            hs[i].emplace_back(t);
        }

        cout << i << " " << tPos.size() << " " << hs[i].size() << endl;
    }
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

