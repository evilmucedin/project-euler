#include "lib/timer.h"

TimerTracker::TimerTracker() : begin_(chrono::high_resolution_clock::now()) {}

double TimerTracker::diffAndReset() {
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - begin_;
    begin_ = end;
    return diff.count();
}

Timer::Timer(std::string m) : m_(m) {}

void Timer::finish() {
    if (!finished_) {
        finished_ = true;
        cerr << m_ << ": " << tt_.diffAndReset() << endl;
    }
}

Timer::~Timer() { finish(); }
