#pragma once

#include "lib/header.h"
#include "lib/noncopyable.h"

class FileIo : NonCopyable {
   public:
    FileIo(string filename, string mode);
    ~FileIo();

    void write(const char* buffer, size_t size);
    size_t read(char* buffer, size_t size);

    template <typename T>
    T readT() {
        T value;
        read(reinterpret_cast<char*>(&value), sizeof(T));
        return value;
    }

    template <typename T>
    bool readT(T& value) {
        return read(reinterpret_cast<char*>(&value), sizeof(T)) == sizeof(T);
    }

    template <typename T>
    bool maybeReadT(T* result) {
        return read(reinterpret_cast<char*>(result), sizeof(T)) == sizeof(T);
    }

    template <typename T>
    void writeT(const T& value) {
        write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    uint64_t tell();
    void seek(uint64_t offset);
    void seekEnd(int64_t offset);
    void setvbuf(size_t size);

    void flush();
    void close();
    bool opened() const;

    size_t printf(const char* fmt, ...);

   private:
    string filename_;
    string mode_;
    FILE* f_{nullptr};
};
