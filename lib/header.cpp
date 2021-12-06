#include "lib/header.h"

#ifndef _MSC_VER

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

namespace std {
string to_string(const string& s) { return s; }
}  // namespace std

string homeDir() { return getpwuid(getuid())->pw_dir; }

#ifdef __SIZEOF_INT128__
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
#else
#ifndef __clang__
ostream& operator<<(ostream& o, u128 v) {
    o << static_cast<unsigned long>(v);
    return o;
}
#endif
#endif

#endif

Exception::Exception() {}

Exception::Exception(string msg) : msg_(move(msg)) {}

const char* Exception::what() const noexcept { return msg_.c_str(); }

