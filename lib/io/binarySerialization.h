#pragma once

#include "lib/io/stream.h"

template<typename T>
bool binarySerialize(OutputStream& s, const T& x);

template<typename T>
bool binaryDeserialize(InputStream& s, T& x);

template<typename T>
inline T binaryDeserialize(InputStream& s) {
    T res;
    ALWAYS_ASSERT(binaryDeserialize(s, res));
    return res;
}
