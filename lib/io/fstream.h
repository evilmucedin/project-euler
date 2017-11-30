#pragma once

#include "lib/header.h"

class IFstream : public ifstream {
   public:
    IFstream() = default;

    IFstream(const string& filename, ios_base::openmode mode = ios_base::in);
    void open(const string& filename, ios_base::openmode mode = ios_base::in);
};

class OFstream : public ofstream {
   public:
    OFstream() = default;

    OFstream(const string& filename, ios_base::openmode mode = ios_base::out);
    void open(const string& filename, ios_base::openmode mode = ios_base::out);
};
