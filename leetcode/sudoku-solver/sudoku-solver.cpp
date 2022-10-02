#include "../header.h"
#include <lib/header.h>

class Solution {
public:
    using Row = vector<char>;
    using Board = vector<Row>;

    struct Cell {
        int x;
        int y;
    };
    using Cells = vector<Cell>;

    struct Goodness {
        vector<int> count_;

        Goodness() : count_(9) {}

        bool add(int x) {
            if (count_[x]) {
                return false;
            }
            ++count_[x];
            return true;
        }

        bool add(char ch) {
            if (ch == '.') {
                return true;
            }
            return add(int(ch - '1'));
        }

        void reset() {
            for (int i = 0; i < 9; ++i) {
                count_[i] = 0;
            }
        }
    };

    bool isGood(Row& r) {
        sort(r.begin(), r.end());
        for (size_t i = 1; i < 9; ++i) {
            if ((r[i - 1] == r[i]) && (r[i] != '.')) {
                return false;
            }
        }
        return true;
    }

    bool isGood(const Board& b) {
        Goodness g;
        for (int i = 0; i < 9; ++i) {
            g.reset();
            for (int j = 0; j < 9; ++j) {
                if (!g.add(b[i][j])) {
                    return false;
                }
            }
        }
        for (int i = 0; i < 9; ++i) {
            g.reset();
            for (int j = 0; j < 9; ++j) {
                if (!g.add(b[j][i])) {
                    return false;
                }
            }
        }
        for (int i1 = 0; i1 < 3; ++i1) {
            for (int j1 = 0; j1 < 3; ++j1) {
                g.reset();
                for (int i2 = 0; i2 < 3; ++i2) {
                    for (int j2 = 0; j2 < 3; ++j2) {
                        if (!g.add(b[3 * i1 + i2][3 * j1 + j2])) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    struct Move {
        int x;
        int y;
        char ch;
    };

    bool findMove(Board& board, Move* move) {
        vector<char> goodChars;
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (board[i][j] == '.') {
                    goodChars.clear();
                    for (char ch = '1'; ch <= '9'; ++ch) {
                        board[i][j] = ch;
                        if (isGood(board)) {
                            goodChars.emplace_back(ch);
                        }
                    }
                    board[i][j] = '.';
                    if (goodChars.size() == 1) {
                        move->x = i;
                        move->y = j;
                        move->ch = goodChars.front();
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void solveSudoku_(Board& board) {
        Move move;
        while (findMove(board, &move)) {
            board[move.x][move.y] = move.ch;
        }
    }

    bool solveSudokuRec(Board& board, const Cells& cells, size_t index) {
        if (index >= cells.size()) {
            return isGood(board);
        }
        if (!isGood(board)) {
            return false;
        }
        const Cell& c = cells[index];
        for (char ch = '1'; ch <= '9'; ++ch) {
            board[c.x][c.y] = ch;
            if (solveSudokuRec(board, cells, index + 1)) {
                return true;
            }
        }
        board[c.x][c.y] = '.';
        return false;
    }

    void solveSudoku(Board& board) {
        Move move;
        while (findMove(board, &move)) {
            board[move.x][move.y] = move.ch;
        }

        Cells empty;
        for (int i = 0; i < board.size(); ++i) {
            for (int j = 0; j < board[i].size(); ++j) {
                if (board[i][j] == '.') {
                    empty.emplace_back(Cell{i, j});
                }
            }
        }

        solveSudokuRec(board, empty, 0);
    }
};

int main() {
    Timer t("Sudoku timer");
    Solution sol;
    Solution::Board board = {
        {'5', '3', '.', '.', '7', '.', '.', '.', '.'}, {'6', '.', '.', '1', '9', '5', '.', '.', '.'},
        {'.', '9', '8', '.', '.', '.', '.', '6', '.'}, {'8', '.', '.', '.', '6', '.', '.', '.', '3'},
        {'4', '.', '.', '8', '.', '3', '.', '.', '1'}, {'7', '.', '.', '.', '2', '.', '.', '.', '6'},
        {'.', '6', '.', '.', '.', '.', '2', '8', '.'}, {'.', '.', '.', '4', '1', '9', '.', '.', '5'},
        {'.', '.', '.', '.', '8', '.', '.', '7', '9'}};
    sol.solveSudoku(board);
    cerr << board << endl;

    return 0;
}
