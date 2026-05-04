#pragma once

#include "lib/header.h"

enum ConsoleCode {
    FG_RED      = 31,
    FG_GREEN    = 32,
    FG_BLUE     = 34,
    FG_DEFAULT  = 39,
    BG_RED      = 41,
    BG_GREEN    = 42,
    BG_BLUE     = 44,
    BG_DEFAULT  = 49
};

class Modifier {
public:
    Modifier(ConsoleCode code);
    ConsoleCode code_;
};

ostream& operator<<(ostream& os, const Modifier& mod);
