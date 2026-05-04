#include "lib/datetime.h"

#include <cstring>

#include "lib/math.h"
#include "lib/string.h"

namespace {
StringVector splitTokens(const string& s) {
    return splitByFunctor(s, [](char ch) { return !isdigit(ch); });
}
}

Date::Date() {}

Date::Date(const string& s) { parse(s); }

void Date::parse(const string& s) {
    auto parts = splitTokens(s);
    ENFORCE_EQ(parts.size(), 3);
    memset(&dt_, 0, sizeof(dt_));
    dt_.tm_year = stoi(parts[0]) - 1900;
    dt_.tm_mon = stoi(parts[1]) - 1;
    dt_.tm_mday = stoi(parts[2]);
}

string Date::str() const {
    char buffer[100];
    sprintf(buffer, "%4d-%2d-%2d", dt_.tm_year + 1900, dt_.tm_mon + 1, dt_.tm_mday);
    return buffer;
}

Time::Time() {}

Time::Time(const string& s) { parse(s); }

int Time::getHours() const { return time_ * kHoursInDay; }

int Time::getMinutes() const { return ((size_t)(time_ * kMinutesInDay)) % kMinutesInHour; }

int Time::getSeconds() const { return ((size_t)(time_ * kSecondsInDay)) % kSecondsInMinute; }

double Time::getSubSeconds() const {
    double seconds = time_ * kSecondsInDay;
    return seconds - ((size_t)seconds);
}

void Time::parse(const string& s) {
    auto parts = splitTokens(s);
    ENFORCE(!parts.empty());
    time_ = 0;
    time_ += ((double)stoi(parts[0])) / kHoursInDay;
    if (parts.size() == 1) {
        return;
    }
    time_ += ((double)stoi(parts[1])) / kMinutesInDay;
    if (parts.size() == 2) {
        return;
    }
    time_ += ((double)stoi(parts[2])) / kSecondsInDay;
    if (parts.size() == 3) {
        return;
    }
    double subseconds = (((double)stoi(parts[3])) / power((size_t)10, parts[3].size()));
    time_ += subseconds / kSecondsInDay;
}

string Time::str() const {
    char buffer[100];
    sprintf(buffer, "%02d:%02d:%02d.%09d", getHours(), getMinutes(), getSeconds(), (int)(getSubSeconds() * 1000000000));
    return buffer;
}

DateTime::DateTime() {}

DateTime::DateTime(const string& s, char delim) { parse(s, delim); }

void DateTime::parse(const string& s, char delim) {
    auto parts = split(s, delim);
    ENFORCE_EQ(parts.size(), 2);
    date_.parse(parts[0]);
    time_.parse(parts[1]);
}

string DateTime::str() const { return date_.str() + " " + time_.str(); }
