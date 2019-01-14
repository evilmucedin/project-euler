#pragma once

#include <chrono>

#include "lib/header.h"

class TimerTracker {
   public:
    TimerTracker();
    double diffAndReset();

   private:
    chrono::time_point<chrono::high_resolution_clock> begin_;
};

class Timer {
   public:
    Timer(std::string m);
    void finish();
    ~Timer();

   private:
    string m_;
    TimerTracker tt_;
    bool finished_{false};
};
