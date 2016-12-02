#include "lib/timer.h"

Timer::Timer(std::string m)
    : m_(m)
    , begin_(chrono::high_resolution_clock::now())
{
}

Timer::~Timer() {
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - begin_;
    cerr << m_ << ": " << diff.count() << endl;
}
