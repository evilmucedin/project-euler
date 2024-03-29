#pragma once

#include "lib/header.h"
#include "lib/string.h"

class File {
   public:
    File(string filename, string mode);

    void close();

    void write(const char* p, size_t len);
    void write(const string& s);

    int getC();
    WChar getWC();
    WChar getUTF8C();
    bool eof();
    u64 offset() const;

   private:
    string filename_;
    string mode_;
    FILE* f_;
};

class BufferedFileReader {
   public:
    BufferedFileReader(File& f);

    bool eof() const;
    WChar peek() const;
    WChar advance();
    u64 offset() const;

   private:
    void tryRead();

    File& f_;
    bool eof_;
    WChar next_;
    size_t offset_;
};

class BufferedStringReader {
   public:
    BufferedStringReader(const WString& s);

    bool eof() const;
    WChar peek() const;
    WChar advance();
    u64 offset() const;
    WChar getUTF8C();

   private:
    void tryRead();

    const WString& s_;
    bool eof_;
    WChar next_;
    size_t offset_;
};
