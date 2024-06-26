#include "glog/logging.h"
#include "lib/header.h"

static constexpr u64 N = 65;
static constexpr u64 M = 10000000000000;
// static constexpr u64 M = 10000;

U64Vector genFib(size_t n) {
    U64Vector result(n);
    result[0] = 1;
    result[1] = 2;
    for (size_t i = 2; i < n; ++i) {
        result[i] = result[i - 1] + result[i - 2];
    }
    return result;
}

/*
i64 naiveFMem(const U64Vector fib, I64Vector& mem, u64 n, size_t minIndex) {
    if (mem[n] == -1) {
        i64 result = 0;
        size_t index = minIndex;
        while (fib[index] <= n) {
            result += naiveFMem(minIndex + 1, fib, mem, n - fib[index]);
            ++index;
        }
        mem[n] = result;
    }
    return mem[n];
}
*/

void gen(const U64Vector& fib, I64Vector& mem, u64 now, u64 index) {
    if (now <= M) {
        ++mem[now];
        for (u64 i = index; i < fib.size(); ++i) {
            gen(fib, mem, now + fib[i], i + 1);
        }
    }
}

u64 genSum(const U64Vector& fib, u64 now, u64 index) {
    u64 result = 0;
    if (now <= M) {
        result += M / now;
        for (u64 i = index; i < fib.size(); ++i) {
            result += genSum(fib, now * fib[i], i + 1);
        }
    }
    return result;
}

u64 genSum2(const U64Vector& fib, const U64Vector& sums, size_t index, i64 remain) {
    if (remain < 0) {
        return 0;
    }
    if (index >= fib.size()) {
        return 1;
    }
    if (sums[index] <= remain) {
        return 1ULL << (sums.size() - index);
    }
    u64 result = 0;
    result += genSum2(fib, sums, index + 1, remain);
    result += genSum2(fib, sums, index + 1, remain - fib[index]);
    LOG_EVERY_MS(INFO, 1000) << OUT(slice(fib, 0, index)) << OUT(remain) << OUT(sums[index]);
    return result;
}

int main() {
    const auto fib = genFib(N);
    LOG(INFO) << OUT(fib) << OUT((fib.back() > M));

    const auto rfib = reversed(fib);
    U64Vector sums(rfib.size());
    {
        for (size_t i = 0; i < rfib.size(); ++i) {
            sums[i] = sum(slice(rfib, i, rfib.size()));
        }
    }

    cout << genSum2(rfib, sums, 0, M) << endl;

    /*
    I64Vector mem(M + 1, 0);
    mem[0] = 0;
    gen(fib, mem, 0, 0);

    u64 sum = 0;
    I64Vector ssum(M + 1, 0);
    for (size_t i = 0; i <= M; ++i) {
        LOG(INFO) << OUT(i) << OUT(mem[i]);
        sum += mem[i];
        ssum[i] = sum;
    }
    LOG(INFO) << OUT(sum);
    LOG(INFO) << OUT(mem);
    LOG(INFO) << OUT(ssum);

    u64 sum2 = genSum(fib, 1, 0);
    LOG(INFO) << OUT(sum2);

    */

    return 0;
}
