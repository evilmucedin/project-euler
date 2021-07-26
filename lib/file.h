#pragma once

#include "lib/header.h"
#include "lib/string.h"

class File {
public:
    File(string filename, string mode);

    void close();

    int getC();
    WChar getWC();
    WChar getUTF8C();
    bool eof();

private:
    string filename_;
    string mode_;
    FILE* f_;
};
