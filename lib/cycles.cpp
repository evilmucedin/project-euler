#include "cycles.h"

ostream& operator<<(ostream& o, const Cycle& c) {
    o << "(" << c.start_ << ", " << c.period_ << ")";
    return o;
}

