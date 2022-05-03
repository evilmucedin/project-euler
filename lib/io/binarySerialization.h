#pragma once

#include "lib/io/stream.h"

template<typename T>
void binarySerialize(OutputStream& s, const T& x);

template<typename T>
void binaryDeserialize(InputStream& s, T& x);

template<typename T>
T binaryDeserialize(InputStream& s);
