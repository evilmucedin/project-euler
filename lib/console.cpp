#include "lib/console.h"

Modifier::Modifier(ConsoleCode code)
    : code_(code)
{
}

ostream& operator<<(ostream& os, const Modifier& mod) {
    return os << "\x1B[" << mod.code_ << "m";
}

