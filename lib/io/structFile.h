#pragma once

#include "lib/io/zstream.h"

template <typename T>
class StructFileReader {
   public:
    StructFileReader(const string& filename) : f_(openZlibFileBufferedReader(filename)), eof_(false) { readNext(); }

    bool eof() const { return eof_; }

    const T& get() const { return value_; }

    bool next(T& value) {
        if (eof_) {
            value = std::move(value_);
            readNext();
            return true;
        }
        return false;
    }

    void next() {
        readNext();
    }

   private:
    void readNext() {
        if (!eof_) {
            eof_ = !f_->readT(value_);
        }
    }

    PInputStream f_;
    bool eof_;
    T value_;
};

template <typename T>
class StructFileWriter {
   public:
    StructFileWriter(const string& filename) : f_(openZlibFileBufferedWriter(filename)) {}

    void write(const T& value) { f_->writeT(value); }

   private:
    POutputStream f_;
};
