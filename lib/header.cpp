#include "lib/header.h"

ostream& operator<<(ostream& o, const U64Vector& v) {
    o << "[";
    bool first = true;
    for (auto n : v) {
        if (!first) {
            o << ", ";
        }
        first = false;
        o << n;
    }
    o << "]";
    return o;
}

ostream& operator<<(ostream& o, const U32Vector& v) {
    o << "[";
    bool first = true;
    for (auto n : v) {
        if (!first) {
            o << ", ";
        }
        first = false;
        o << n;
    }
    o << "]";
    return o;
}

ostream& operator<<(ostream& o, u128 v) {
    u64 high = v >> 64;
    u64 low = (v << 64) >> 64;
    if (!high) {
        o << low;
    } else {
        o << "(" << high << ", " << low << ")";
    }
    return o;
}

ostream& operator<<(ostream& o, i128 v) {
    if (v < 0) {
        o << "-";
        v = -v;
    }
    o << static_cast<u128>(v);
    return o;
}
