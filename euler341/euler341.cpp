#include <cstdio>

#include <vector>

using namespace std;

typedef long long int ui64;
typedef vector<ui64> TIntVector;

static const ui64 initGolomb2[] = {0, 1, 2};
TIntVector golomb2(initGolomb2, initGolomb2 + 3);

static const ui64 N = 1000;
static const ui64 N3 = N*N*N;
static const ui64 N18 = 1000000000000000000ULL;

ui64 golomb_(ui64 n);

ui64 golomb2__(ui64 n) {
    while (n >= golomb2.size()) {
        golomb2.push_back( golomb2.back() + (golomb2.size() - 1)*golomb_(golomb2.size() - 1) );
    }
    return golomb2[n];
}

ui64 golomb2_(ui64 n) {
    while (n >= golomb2.back()) {
        golomb2__(golomb2.size());
    }

    size_t left = 0;
    size_t right = golomb2.size() - 1;
    while (left < right) {
        const size_t mid = (left + right) / 2;
        const ui64 val = golomb2[mid];
        if (val == n)
            return mid;
        else if (val > n)
            right = mid;
        else
            left = mid + 1;
    }
    while (n >= golomb2[left])
        --left;
    return left - 1;
}

static const ui64 initCache[] = {0, 1, 2};
TIntVector cache(initCache, initCache + 3);

void FillCache() {
    static const ui64 CACHE_SIZE = 2000000000;
    cache.resize(CACHE_SIZE);
    cache[1] = 1;
    for (size_t i = 2; i < CACHE_SIZE; ++i)
        cache[i] = 1 + cache[i - cache[cache[i - 1]]];
}

ui64 golomb_(ui64 n) {
    ui64 res = 0;
    while (n >= cache.size()) {
        ui64 curGolomb = golomb2_(n - 1);
        ui64 limit = golomb2[curGolomb];
        ui64 d = (n - limit + curGolomb - 1)/curGolomb;
        // printf("%lld %lld\n", curGolomb, d);
        if (d > 0) {
            n -= d*curGolomb;
            res += d;
        } else {
            n -= curGolomb;
            ++res;
        }
    }
    return res + cache[n];
}

int main() {
    printf("%lld %lld", golomb_(1000), golomb_(1000000));
    FillCache();
    printf(" %lld", golomb2_(N18));
    {
        ui64 res = 0;
        for (ui64 i = 1; i < 1000; ++i) {
            res += golomb_(i*i*i);
        }
        printf(" %lld\n", res);
    }
    {
        ui64 res = 0;
        for (ui64 i = 1; i < 1000000; ++i) {
            if ((i % 100) == 0) {
                printf("%lld %.2lf %lld\n", i, 100.0*double(i)/1000000, res);
                fflush(stdout);
            }
            res += golomb_(i*i*i);
        }
        printf("%lld\n", res);
    }
    return 0;
}
