#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

static const int MIN_T_X = 179;
static const int MAX_T_X = 201;
static const int MIN_T_Y = -109;
static const int MAX_T_Y = -63;
/*
*/

/*
static const int MIN_T_X = 20;
static const int MAX_T_X = 30;
static const int MIN_T_Y = -10;
static const int MAX_T_Y = -5;
*/

bool inT(int x, int y) {
    return x >= MIN_T_X && x <= MAX_T_X && y >= MIN_T_Y && y <= MAX_T_Y;
}

void first() {
    int maxY = -1000000000;
    size_t count = 0;
    for (int vx0 = 0; vx0 < 1000; ++vx0) {
        for (int vy0 = -1000; vy0 < 1000; ++vy0) {
            int x = 0;
            int y = 0;
            int vx = vx0;
            int vy = vy0;
            int mxY = -1000000000;
            bool good = false;
            while (y > -1000) {
                if (inT(x, y)) {
                    good = true;
                }
                mxY = max(mxY, y);

                x += vx;
                y += vy;
                if (vx > 0) {
                    --vx;
                }
                --vy;
            }

            if (good) {
                ++count;
            }

            if (good && mxY > maxY) {
                maxY = mxY;
            }
        }
    }

    cout << maxY << endl;
    cout << count << endl;
}

void second() {
    first();
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

