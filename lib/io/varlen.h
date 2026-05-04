#pragma once

#include "fileIo.h"

template<typename T>
void writeVarLen(File& f, T value) {
    if (!value) {
        f.writeT<uint8_t>(0);
        return;
    }
    while (value) {
        uint8_t next = value & 127;
        value >>= 7;
        if (value) {
            next += 128;
        }
        f.writeT(next);
    }
}

template<typename T>
T readVarLen(File& f) {
    T result = 0;
    while (true) {
        uint8_t next = f.readT<uint8_t>();
        result = result << 7;
        if (next & 128) {
            result += next & 127;
        } else {
            result += next;
            return result;
        }
    }
}

template<typename T>
bool maybeReadVarLen(File& f, T& result) {
    result = 0;
    while (true) {
        uint8_t next;
        if (!f.maybeReadT(&next)) {
            return false;
        }
        result = result << 7;
        if (next & 128) {
            result += next & 127;
        } else {
            result += next;
            return true;
        }
    }
}
