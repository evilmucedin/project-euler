class Solution {
public:
    int totalNQueens(int n) {
        int count = 0;
        solve(n, 0, 0ULL, 0ULL, 0ULL, count);
        return count;
    }
private:
    void solve(int n, int row, unsigned long long cols, unsigned long long diags, unsigned long long anti, int &count) {
        if (row == n) { ++count; return; }
        unsigned long long mask = (n >= 64) ? ~0ULL : ((1ULL << n) - 1ULL);
        unsigned long long avail = mask & ~(cols | diags | anti);
        while (avail) {
            unsigned long long bit = avail & -avail;
            avail -= bit;
            solve(n, row + 1, cols | bit, (diags | bit) << 1, (anti | bit) >> 1, count);
        }
    }
};
