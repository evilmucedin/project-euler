#pragma once

#include <thread>

#include "lib/header.h"

template<typename T>
void runInThreads(int nThreads, T& lambda) {
    vector<thread> threads;
    for (int i = 0; i < nThreads; ++i) {
        threads.emplace_back( thread([&, i] {
            lambda(i);
        } ));
    }
    for (auto& t : threads) {
        t.join();
    }
}
