#pragma once

#include <chrono>

#include "lib/header.h"

class Timer {
public:
    Timer(std::string m);
    ~Timer();

private:
    string m_;
    chrono::time_point<chrono::high_resolution_clock> begin_;
};
