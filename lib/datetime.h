#pragma once

#include <ctime>

#include "lib/header.h"

struct Date {
    Date();
    Date(const string& s);
    void parse(const string& s);
    string str() const;

    tm dt_;
};

struct Time {
    Time();
    Time(const string& s);
    void parse(const string& s);
    string str() const;
    int getHours() const;
    int getMinutes() const;
    int getSeconds() const;
    double getSubSeconds() const;

    double time_;

    static constexpr size_t kHoursInDay = 24;
    static constexpr size_t kMinutesInHour = 60;
    static constexpr size_t kMinutesInDay = kHoursInDay * kMinutesInHour;
    static constexpr size_t kSecondsInMinute = 60;
    static constexpr size_t kSecondsInDay = kMinutesInDay * kSecondsInMinute;
};

struct DateTime {
    DateTime();
    DateTime(const string& s, char delim = 'T');
    void parse(const string& s, char delim = 'T');
    string str() const;

    Date date_;
    Time time_;
};
