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
        Row v(9);
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                v[j] = b[i][j];
            }
            if (!isGood(v)) {
                return false;
            }
        }
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                v[j] = b[j][i];
            }
            if (!isGood(v)) {
                return false;
            }
        }
        for (int i1 = 0; i1 < 3; ++i1) {
            for (int j1 = 0; j1 < 3; ++j1) {
                int pos = 0;
                for (int i2 = 0; i2 < 3; ++i2) {
                    for (int j2 = 0; j2 < 3; ++j2) {
                        v[pos++] = b[3 * i1 + i2][3 * j1 + j2];
                    }
                }
                if (!isGood(v)) {
                    return false;
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
        for (char ch = '1'; ch <= '9'; ++ch) {
            board[cells[index].x][cells[index].y] = ch;
            if (solveSudokuRec(board, cells, index + 1)) {
                return true;
            }
        }
        board[cells[index].x][cells[index].y] = '.';
        return false;
    }

    void solveSudoku(Board& board) {
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
