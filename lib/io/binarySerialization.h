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

template<typename T>
bool binarySerialize(OutputStream& s, const vector<T>& v) {
    const u64 size = v.size();
    s.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (size_t i = 0; i < size; ++i) {
        if (!binarySerialize(s, v[i])) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool binaryDeserialize(InputStream& s, vector<T>& v) {
    u64 size;
    if (!s.read(reinterpret_cast<char*>(&size), sizeof(size))) {
        return false;
    }
    v.resize(size);
    for (size_t i = 0; i < size; ++i) {
        if (!binaryDeserialize(s, v[i])) {
            return false;
        }
    }
    return true;
}
