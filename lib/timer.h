#pragma once

#include <chrono>

#include "lib/header.h"

class Timer {
public:
    Timer(std::string m);
    void finish();
    ~Timer();

private:
    string m_;
    chrono::time_point<chrono::high_resolution_clock> begin_;
    bool finished_{false};
};
