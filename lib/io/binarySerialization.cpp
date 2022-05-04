#include "binarySerialization.h"

#define OUT_INT_TEMPLATE(TYPENAME)                                               \
    template <>                                                                  \
    bool binarySerialize<TYPENAME>(OutputStream & stream, const TYPENAME& x) {   \
        stream.write(reinterpret_cast<const char*>(&x), sizeof(x));              \
        return true;                                                             \
    }                                                                            \
                                                                                 \
    template <>                                                                  \
    bool binaryDeserialize<TYPENAME>(InputStream & stream, TYPENAME & x) {       \
        return stream.read(reinterpret_cast<char*>(&x), sizeof(x)) == sizeof(x); \
    }

OUT_INT_TEMPLATE(i8)
OUT_INT_TEMPLATE(u8)
OUT_INT_TEMPLATE(i16)
OUT_INT_TEMPLATE(u16)
OUT_INT_TEMPLATE(i32)
OUT_INT_TEMPLATE(u32)
OUT_INT_TEMPLATE(i64)
OUT_INT_TEMPLATE(u64)

#undef OUT_INT_TEMPLATE

template <>
bool binarySerialize<string>(OutputStream& s, const string& x) {
    bool res = binarySerialize<u64>(s, x.size());
    if (res) {
        s.write(x.data(), x.size() + 1);
        return true;
    }
    return false;
}

template <>
bool binaryDeserialize<string>(InputStream& s, string& x) {
    u64 size = 0;
    if (binaryDeserialize(s, size)) {
        x.resize(size);
        return s.read(x.data(), size + 1);
    }
    return false;
}
