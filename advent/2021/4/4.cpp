#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

struct Cell {
    int n;
    bool marked = false;
};

using Board = vector<vector<Cell>>;

bool readBoard(Board& board) {
    board.clear();
    board.resize(5, vector<Cell>(5));
    int n = -1;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            n = -1;
            cin >> n;
            if (n == -1) {
                return false;
            }
            board[i][j].n = n;
        }
    }
    return true;
}

void mark(Board& b, int num) {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (b[i][j].n == num) {
                b[i][j].marked = true;
            }
        }
    }
}

bool isMarked(const Board& b) {
    for (int i = 0; i < 5; ++i) {
        int j = 0;
        while (j < 5 && b[i][j].marked) {
            ++j;
        }
        if (j == 5) {
            return true;
        }
    }
    for (int i = 0; i < 5; ++i) {
        int j = 0;
        while (j < 5 && b[j][i].marked) {
            ++j;
        }
        if (j == 5) {
            return true;
        }
    }
    return false;
}

int notMarked(const Board& b) {
    int sum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (!b[i][j].marked) {
                sum += b[i][j].n;
            }
        }
    }
    return sum;
}

void first() {
    string s;
    cin >> s;
    const auto parts = split(s, ',');

    IntVector iParts;
    for (const auto& p: parts) {
        iParts.emplace_back(stoi(p));
    }

    vector<Board> boards;
    Board board;
    while (readBoard(board)) {
        boards.emplace_back(board);
    }

    i64 result = -1;
    for (const auto& num: iParts) {
        if (result != -1) {
            break;
        }
        for (auto& b: boards) {
            mark(b, num);
            if (isMarked(b)) {
                result = num * notMarked(b);
                break;
            }
        }
    }

    cout << result << endl;
}

void second() {
    string s;
    cin >> s;
    const auto parts = split(s, ',');

    IntVector iParts;
    for (const auto& p: parts) {
        iParts.emplace_back(stoi(p));
    }

    vector<Board> boards;
    Board board;
    while (readBoard(board)) {
        boards.emplace_back(board);
    }

    BoolVector was(boards.size());
    for (const auto& num: iParts) {
        for (size_t iBoard = 0; iBoard < boards.size(); ++iBoard) {
            if (was[iBoard]) {
                continue;
            }
            auto& b = boards[iBoard];
            mark(b, num);
            if (isMarked(b)) {
                cout << num * notMarked(b) << endl;
                was[iBoard] = true;
            }
        }
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

