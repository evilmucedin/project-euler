#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

struct Line {
    bool on;
    int minX;
    int maxX;
    int minY;
    int maxY;
    int minZ;
    int maxZ;

    bool in(int x, int y, int z) const {
        return (x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY) && (z >= minZ) && (z <= maxZ);
    }

    bool in2(int x, int y) const { return (x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY); }
};

using Lines = vector<Line>;

Lines input() {
    const auto input = readInputLines();
    Lines result;
    for (const auto& s : input) {
        const auto parts = split(s, ' ');
        ASSERTEQ(parts.size(), 2);
        Line l;
        l.on = parts[0] == "on";
        const auto ints = parseIntegers(parts[1]);
        ASSERTEQ(ints.size(), 6);
        l.minX = ints[0];
        l.maxX = ints[1];
        l.minY = ints[2];
        l.maxY = ints[3];
        l.minZ = ints[4];
        l.maxZ = ints[5];
        result.emplace_back(std::move(l));
    }
    return result;
}

void second() {
    const auto cubes = input();

    IntVector xx;
    IntVector yy;
    IntVector zz;

    for (const auto& l : cubes) {
        for (int i = -1; i <= 1; ++i) {
            xx.emplace_back(l.minX + i);
            xx.emplace_back(l.maxX + i);
            yy.emplace_back(l.minY + i);
            yy.emplace_back(l.maxY + i);
            zz.emplace_back(l.minZ + i);
            zz.emplace_back(l.maxZ + i);
        }
    }

    sortAndUnique(xx);
    sortAndUnique(yy);
    sortAndUnique(zz);

    using TT = u64;

    TT result = 0;
    Lines subCubes;
    for (int i = 1; i < xx.size(); ++i) {
        // int x = (xx[i - 1] + xx[i]) / 2;
        int x = xx[i - 1];
        for (int j = 1; j < yy.size(); ++j) {
            // int y = (yy[j - 1] + yy[j]) / 2;
            int y = yy[j - 1];

            subCubes.clear();
            for (const auto& c : cubes) {
                if (c.in2(x, y)) {
                    subCubes.emplace_back(c);
                }
            }
            reverse(subCubes);
            TT xy = TT(xx[i] - xx[i - 1]) * TT(yy[j] - yy[j - 1]);

            LOG_EVERY_MS(INFO, 1000) << i << " " << xx.size() << " " << j << " " << yy.size() << " " << subCubes.size();

            for (int k = 1; k < zz.size(); ++k) {
                // int z = (zz[k - 1] + zz[k]) / 2;
                int z = zz[k - 1];
                bool on = false;
                for (const auto& c : subCubes) {
                    if (z >= c.minZ && z <= c.maxZ) {
                        on = c.on;
                        break;
                    }
                }
                if (on) {
                    result += xy * TT(zz[k] - zz[k - 1]);
                }
            }
        }
    }

    cout << "2: " << result << endl;
}

void first() {
    const auto cubes = input();

    vector<vector<vector<bool>>> state(101, vector<vector<bool>>(101, vector<bool>(101)));

    for (const auto& l : cubes) {
        for (int x = l.minX; x <= l.maxX; ++x) {
            if (x >= -50 && x <= 50) {
                for (int y = l.minY; y <= l.maxY; ++y) {
                    if (y >= -50 && y <= 50) {
                        for (int z = l.minZ; z <= l.maxZ; ++z) {
                            if (z >= -50 && z <= 50) {
                                int xx = x + 50;
                                int yy = y + 50;
                                int zz = z + 50;
                                state[xx][yy][zz] = l.on;
                            }
                        }
                    }
                }
            }
        }
    }

    size_t result = 0;
    for (const auto& v1 : state) {
        for (const auto& v2 : v1) {
            for (const auto& v3 : v2)
                if (v3) {
                    ++result;
                }
        }
    }
    cout << "1: " << result << endl;
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
