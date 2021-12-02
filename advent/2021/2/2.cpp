#include "gflags/gflags.h"
#include "lib/header.h"
#include "lib/init.h"

DEFINE_int32(test, 1, "test number");

void first() {
    // cerr << "first" << endl;
    string cmd;
    int d;

    i64 x = 0;
    i64 y = 0;

    cin >> cmd >> d;
    while (!cmd.empty()) {
        if (cmd == "forward") {
            x += d;
        } else if (cmd == "down") {
            y += d;
        } else if (cmd == "up") {
            y -= d;
        }

        cmd.clear();
        cin >> cmd >> d;
    }

    cout << x << endl;
    cout << y << endl;
    cout << x * y << endl;
}

void second() {
    // cerr << "second" << endl;
    string cmd;
    int d;

    i64 x = 0;
    i64 y = 0;
    i64 aim = 0;

    cin >> cmd >> d;
    while (!cmd.empty()) {
        if (cmd == "forward") {
            x += d;
            y += d * aim;
        } else if (cmd == "down") {
            aim += d;
        } else if (cmd == "up") {
            aim -= d;
        }

        cmd.clear();
        cin >> cmd >> d;
    }

    cout << x << endl;
    cout << y << endl;
    cout << x * y << endl;
}

int main(int argc, char* argv[]) {
    /*
    cerr << argc << endl;
    for (int i = 1; i < argc; ++i) {
        cerr << i << " " << argv[i] << endl;
    }
    */

    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }

    return 0;
}
