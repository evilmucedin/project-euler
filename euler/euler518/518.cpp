#include <cmath>

#include <thread>

#include "lib/primes.h"

void solve(u32 n) {
    Erato erato(n);
    u64 result = 0;

    static constexpr int kThreads = 4;

    vector<thread> threads(kThreads);
    for (size_t iThread = 0; iThread < kThreads; ++iThread) {
        threads[iThread] = thread( [&, iThread] {
            u64 localResult = 0;
            const auto& primes = erato.primes_;
            for (size_t i = iThread; i < primes.size(); i += kThreads) {
                size_t j;
                for (j = i + 1; j < primes.size(); ++j) {
                    double alpha = static_cast<double>(primes[j] + 1)/(primes[i] + 1);
                    double dc = (primes[j] + 1)*alpha;
                    u32 c = static_cast<u32>(dc + 0.4999);
                    if (c + 1 >= n) {
                        break;
                    }
                    if (fabs(dc - c) < 0.001) {
                        if (erato.isPrime(c - 1)) {
                            localResult += primes[i];
                            localResult += primes[j];
                            localResult += c - 1;
                            // cerr << erato.primes_[i] << " " << erato.primes_[j] << " " << (c - 1) << endl;
                        }
                    }
                }
                if (0 == (i % 1000)) {
                    cerr << "..." << i << " " << j << endl;
                }
            }
            result += localResult;
            cerr << iThread << " is done" << endl;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << n << " " << result << endl;
}

int main() {
    solve(100);
    solve(100000000);

    return 0;
}
