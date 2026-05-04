#pragma once

#include "lib/header.h"

class IFStream : public ifstream {
   public:
    IFStream() = default;

    IFStream(const string& filename, ios_base::openmode mode = ios_base::in);
    void open(const string& filename, ios_base::openmode mode = ios_base::in);
    string readLine();
    bool readLine(string& line);
};

class OFStream : public ofstream {
   public:
    OFStream() = default;

    OFStream(const string& filename, ios_base::openmode mode = ios_base::out);
    void open(const string& filename, ios_base::openmode mode = ios_base::out);
};
