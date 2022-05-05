#pragma once

#include "lib/io/zstream.h"

template <typename T>
class StructFileReader {
   public:
    StructFileReader(const string& filename) : f_(openZlibFileBufferedReader(filename)) {}

    bool next(T& result) { return f_->readT(result); }

   private:
    PInputStream f_;
};

template <typename T>
class StructFileWriter {
   public:
    StructFileWriter(const string& filename) : f_(openZlibFileBufferedWriter(filename)) {}

    void write(const T& value) { f_->writeT(value); }

   private:
    POutputStream f_;
};
