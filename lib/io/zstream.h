#pragma once

#include "lib/header.h"

class ZIfStream : public ifstream {
public:
    ZIfStream(shared_ptr<ifstream> stream);
private:
    shared_ptr<ifstream> stream_;
};
