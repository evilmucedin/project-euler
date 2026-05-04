#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

struct Cell {
    int e{};
    bool f{};
};

using Cells = vector<Cell>;
using Matrix = vector<Cells>;

void first() {
    const auto input = readInputLines();

    Matrix m(10, Cells(10));
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            m[i][j].e = input[i][j] - '0';
        }
    }

    size_t count = 0;
    for (int step = 0; step < 100; ++step) {
        size_t stepCount = 0;

        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                ++m[i][j].e;
                m[i][j].f = false;
            }
        }

        bool change = true;
        while (change) {
            change = false;

            for (int i = 0; i < 10; ++i) {
                for (int j = 0; j < 10; ++j) {
                    if (m[i][j].e > 9 && !m[i][j].f) {
                        m[i][j].f = true;
                        change = true;
                        for (int ii = -1; ii <= 1; ++ii) {
                            for (int jj = -1; jj <= 1; ++jj) {
                                if (ii || jj) {
                                    int iii = i + ii;
                                    int jjj = j + jj;
                                    if (iii >= 0 && iii < 10 && jjj >= 0 && jjj < 10) {
                                        ++m[iii][jjj].e;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                if (m[i][j].f) {
                    ++count;
                    ++stepCount;
                    m[i][j].f = false;
                    m[i][j].e = 0;
                }
            }
        }

        /*
        cerr << step << " " << stepCount << endl;
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                cerr << m[i][j].e;
            }
            cerr << endl;
        }
        */
    }

    cout << count << endl;
}

void second() {
    const auto input = readInputLines();

    Matrix m(10, Cells(10));
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            m[i][j].e = input[i][j] - '0';
        }
    }

    size_t count = 0;
    int step = 0;
    while (true) {
        size_t stepCount = 0;

        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                ++m[i][j].e;
                m[i][j].f = false;
            }
        }

        bool change = true;
        while (change) {
            change = false;

            for (int i = 0; i < 10; ++i) {
                for (int j = 0; j < 10; ++j) {
                    if (m[i][j].e > 9 && !m[i][j].f) {
                        m[i][j].f = true;
                        change = true;
                        for (int ii = -1; ii <= 1; ++ii) {
                            for (int jj = -1; jj <= 1; ++jj) {
                                if (ii || jj) {
                                    int iii = i + ii;
                                    int jjj = j + jj;
                                    if (iii >= 0 && iii < 10 && jjj >= 0 && jjj < 10) {
                                        ++m[iii][jjj].e;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                if (m[i][j].f) {
                    ++count;
                    ++stepCount;
                    m[i][j].f = false;
                    m[i][j].e = 0;
                }
            }
        }

        /*
        cerr << step << " " << stepCount << endl;
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                cerr << m[i][j].e;
            }
            cerr << endl;
        }
        */

        if (stepCount == 100) {
            break;
        }

        ++step;
    }

    cout << step + 1 << endl;
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

