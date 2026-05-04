#include <set>

#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

static const int dx[] = {1, -1, 0, 0, 0, 0};
static const int dy[] = {0, 0, 1, -1, 0, 0};
static const int dz[] = {0, 0, 0, 0, 1, -1};

void first() {
    const auto input = readInputLines();

    using TI = tuple<int, int, int>;
    set<TI> cubes;  // use unordered_set to store the cubes

    int mn = 10000;
    int mx = 0;
    for (const auto& line: input) {                   // read the input
        const auto parts = parseIntegers(line);
        for (auto p: parts) {
            mn = min(mn, p);
            mx = max(mx, p);
        }
        cubes.insert({parts[0], parts[1], parts[2]});  // add the cube to the set
    }
    cerr << "[" << mn << ", " << mx << "]" << endl;

    set<TI> external;
    queue<TI> q;
    q.emplace(TI(30, 30, 30));
    while (!q.empty()) {
        auto cube = q.front();
        q.pop();
        external.emplace(cube);
        int x = get<0>(cube);
        int y = get<1>(cube);
        int z = get<2>(cube);

        for (int i = 0; i < 6; i++) {  // check each of the six sides
            if (x + dx[i] >= -1 && x + dx[i] < 40) {
                if (y + dy[i] >= -1 && y + dy[i] < 40) {
                    if (z + dx[i] >= -1 && z + dz[i] < 40) {
                        TI nc{x + dx[i], y + dy[i], z + dz[i]};
                        if (cubes.count(nc) == 0 && external.count(nc) == 0) {  // if the neighbor is not in the set
                            q.emplace(nc);           // add 1 to the surface area
                            external.emplace(nc);
                        }
                    }
                }
            }
        }
    }

    int surface_area = 0;
    int ext_surface_area = 0;
    for (const TI& cube : cubes) {  // iterate through all the cubes
        int x = get<0>(cube);
        int y = get<1>(cube);
        int z = get<2>(cube);

        for (int i = 0; i < 6; i++) {  // check each of the six sides
            TI nc{x + dx[i], y + dy[i], z + dz[i]};
            if (cubes.count(nc) == 0) {  // if the neighbor is not in the set
                surface_area++;                // add 1 to the surface area
                if (external.count(nc)) {
                    ++ext_surface_area;
                }
            }
        }
    }

    cout << surface_area << endl;
    cout << ext_surface_area << endl;
}

void second() {
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

