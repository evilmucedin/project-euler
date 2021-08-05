#pragma once

#include "lib/header.h"
#include "lib/string.h"

class File {
public:
    File(string filename, string mode);

    void close();

    void write(const char* p, size_t len);
    void write(const string& s);

    int getC();
    WChar getWC();
    WChar getUTF8C();
    bool eof();

private:
    string filename_;
    string mode_;
    FILE* f_;
};
