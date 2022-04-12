#pragma once

#include <memory>

#include "lib/io/file.h"

using namespace std;

class InputStream {
   public:
    virtual ~InputStream();

    virtual size_t read(char* buffer, size_t toRead) = 0;
};

using PInputStream = std::shared_ptr<InputStream>;

class OutputStream {
   public:
    virtual ~OutputStream();

    virtual void write(const string& s);
    virtual void write(char ch);
    virtual void write(const char* buffer, size_t toWrite) = 0;
    virtual void flush() = 0;
};

using POutputStream = std::shared_ptr<OutputStream>;

class StdInputStream : public InputStream {
   public:
    size_t read(char* buffer, size_t toRead) override;
};

class StdOutputStream : public OutputStream {
   public:
    void write(const char* buffer, size_t toWrite) override;
    void flush() override;
};

class FileOutputStream : public OutputStream {
   public:
    FileOutputStream(const string& filename);
    void write(const char* buffer, size_t toWrite) override;
    void flush() override;

   private:
    string filename_;
    File file_;
};

class BufferedInputStream : public InputStream {
   public:
    static constexpr size_t DEFAULT_BUFFER_SIZE = 1 << 20;
    BufferedInputStream(PInputStream nested, size_t bufferSize = DEFAULT_BUFFER_SIZE);
    ~BufferedInputStream();
    size_t read(char* buffer, size_t toRead) override;

   private:
    void refill();

    PInputStream nested_;
    size_t bufferSize_;
    unique_ptr<char[]> buffer_;
    size_t bufferPos_;
    size_t bufferEnd_;
};

class BufferedOutputStream : public OutputStream {
   public:
    static constexpr size_t DEFAULT_BUFFER_SIZE = 1 << 20;
    BufferedOutputStream(POutputStream nested, size_t bufferSize = DEFAULT_BUFFER_SIZE);
    ~BufferedOutputStream();

    void write(const char* buffer, size_t toWrite) override;
    void flush() override;

   private:
    POutputStream nested_;
    size_t bufferSize_;
    unique_ptr<char[]> buffer_;
    size_t bufferPos_;
};

template <typename T>
OutputStream& operator<<(OutputStream& stream, const T& x) {
    stream.write(to_string(x));
    return stream;
}

template <>
inline OutputStream& operator<<<int>(OutputStream& stream, const int& x) {
    thread_local char buffer[128];
    const size_t len = numToBuffer<int, 10>(x, buffer);
    stream.write(buffer, len);
    return stream;
}

template <>
inline OutputStream& operator<<<string>(OutputStream& stream, const string& s) {
    stream.write(s);
    return stream;
}
