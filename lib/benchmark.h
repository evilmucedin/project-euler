#pragma once

#include "lib/header.h"

#include "lib/stat.h"
#include "lib/timer.h"

template <typename T>
void benchmark(const std::string& name, T f) {
    size_t n = 128;
    Stat<double> stat;
    f();
    do {
        TimerTracker tt;
        for (size_t i = 0; i < n; ++i) {
            f();
            stat.add(tt.diffAndReset());
        }
        n *= 2;
        cout << "Benchmark '" << name << "': " << stat.mean() << " (+-" << stat.stddev() << ") after " << stat.count()
             << " iterations" << endl;
    } while (10 * stat.stddev() > stat.mean());
    cout << "Final benchmark '" << name << "': " << stat.mean() << " after " << stat.count() << " iterations" << endl;
}
