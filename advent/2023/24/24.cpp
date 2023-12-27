#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

struct Hailstone {
    long long x, y, z, vx, vy, vz;

    bool on2(long long x2, long long y2) const {
        long long tx = (x2 - x) / vx;
        long long ty = (y2 - y) / vy;
        return tx == ty;
    }
};

// Function to parse input lines and extract hailstone data
Hailstone parseHailstone(const std::string& line) {
    Hailstone hailstone;
    char comma;
    std::istringstream iss(line);
    iss >> hailstone.x >> comma >> hailstone.y >> comma >> hailstone.z
        >> comma >> hailstone.vx >> comma >> hailstone.vy >> comma >> hailstone.vz;
    return hailstone;
}

// Function to check if two hailstones will intersect within the test area
bool willIntersect(const Hailstone& a, const Hailstone& b, long long t) {
    // Calculate the positions of hailstones a and b at time t
    long long ax = a.x + a.vx * t;
    long long ay = a.y + a.vy * t;
    long long bx = b.x + b.vx * t;
    long long by = b.y + b.vy * t;

    // Check if the positions intersect within the test area
    return ((ax >= 200000000000000LL && ax <= 400000000000000LL && ay >= 200000000000000LL && ay <= 400000000000000LL &&
             bx >= 200000000000000LL && bx <= 400000000000000LL && by >= 200000000000000LL &&
             by <= 400000000000000LL) &&
            ((ax == bx && ay == by) || b.on2(ax, ay) || a.on2(bx, by)));
}

int countIntersections(const std::vector<Hailstone>& hailstones) {
    int intersections = 0;
    for (size_t i = 0; i < hailstones.size(); ++i) {
        for (size_t j = i + 1; j < hailstones.size(); ++j) {
            // Calculate the time at which two hailstones might intersect on X and Y axes
            long long t = 0;
            if (hailstones[i].vx != hailstones[j].vx || hailstones[i].vy != hailstones[j].vy) {
                if (hailstones[i].vx != hailstones[j].vx) {
                    t = (hailstones[j].x - hailstones[i].x) / (hailstones[i].vx - hailstones[j].vx);
                } else {
                    t = (hailstones[j].y - hailstones[i].y) / (hailstones[i].vy - hailstones[j].vy);
                }
            }
            if (t >= 0 && willIntersect(hailstones[i], hailstones[j], t)) {
                intersections++;
            }
        }
    }
    return intersections;
}

void first() {
    const auto input = readInputLines();

    std::vector<Hailstone> hailstones;
    std::string line;

    // Read input file and parse hailstone data
    for (const auto& line: input) {
        hailstones.push_back(parseHailstone(line));
    }

    for (const auto& h: hailstones) {
        LOG(INFO) << OUT(h.x) << OUT(h.y) << OUT(h.z) << OUT(h.vx);
    }

    // Count intersections within the specified test area
    int intersectCount = countIntersections(hailstones);

    cout << intersectCount << endl;
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

