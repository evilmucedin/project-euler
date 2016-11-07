#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    static constexpr int N = 100;

    vector<vector<uint64_t>> pointsInTriangle(N + 1, vector<uint64_t>(N + 1, 0));
    for (uint64_t x = 1; x <= N; ++x) {
        for (uint64_t y = 1; y <= N; ++y) {
            uint64_t& count = pointsInTriangle[x][y];
            for (uint64_t i = 1; i <= x; ++i) {
                for (uint64_t j = 1; j <= y; ++j) {
                    if (j*x + i*y < x*y) {
                        ++count;
                    }
                }
            }
        }
    }

    uint64_t count = 0;
    for (uint64_t a = 1; a <= N; ++a) {
        for (uint64_t b = 1; b <= N; ++b) {
            for (uint64_t c = 1; c <= N; ++c) {
                for (uint64_t d = 1; d <= N; ++d) {
                    uint64_t square = pointsInTriangle[a][b] + pointsInTriangle[b][c] + pointsInTriangle[c][d] + pointsInTriangle[d][a] + a + c + b + d - 3;
                    uint64_t root = sqrt((long double)square);
                    if (square == root*root) {
                        ++count;
                    }
                }
            }
        }
    }
    cout << count << endl;
    return 0;
}
