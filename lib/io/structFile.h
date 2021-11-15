#pragma once

#include "lib/io/file.h"

template <typename T>
class StructFileReader {
   public:
    StructFileReader(const string& filename) : f_(filename, "rb") {}

    bool next(T& result) { return f_.readT(result); }

   private:
    File f_;
};

template <typename T>
class StructFileWriter {
   public:
    StructFileWriter(const string& filename) : f_(filename, "wb") {}

    void write(const T& value) { f_.writeT(value); }

   private:
    File f_;
};
