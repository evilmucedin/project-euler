#pragma once

#include <memory>

#include "lib/io/file.h"

using namespace std;

class InputStream {
   public:
    virtual ~InputStream();

    virtual size_t read(char* buffer, size_t toRead) = 0;

    virtual bool readChar(char& ch);
    virtual bool readTo(string& s, char ch);
    virtual bool readLine(string& s);
    virtual string readLine();
};

using PInputStream = std::shared_ptr<InputStream>;

class OutputStream {
   public:
    virtual ~OutputStream();

    virtual void write(const char* buffer, size_t toWrite) = 0;

    virtual void write(const string& s);
    virtual void write(char ch);
    virtual void flush() = 0;
};

using POutputStream = std::shared_ptr<OutputStream>;

typedef void (*StreamManipulator)(OutputStream &);

static inline OutputStream &operator<<(OutputStream &o, StreamManipulator m) {
  m(o);

  return o;
}

void Endl(OutputStream &o);

class StdInputStream : public InputStream {
   public:
    size_t read(char* buffer, size_t toRead) override;
};

class StdOutputStream : public OutputStream {
   public:
    void write(const char* buffer, size_t toWrite) override;
    void flush() override;
};

class FileInputStream : public InputStream {
   public:
    FileInputStream(const string& filename);
    ~FileInputStream();

    size_t read(char* buffer, size_t toRead) override;

   private:
    string filename_;
    int fd_{-1};
};

class FileOutputStream : public OutputStream {
   public:
    FileOutputStream(const string& filename);
    ~FileOutputStream();
    void write(const char* buffer, size_t toWrite) override;
    void flush() override;

   private:
    string filename_;
    int fd_{-1};
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

class InputStringStream : public InputStream {
   public:
    InputStringStream(string s);
    size_t read(char* buffer, size_t toRead) override;

   private:
    string s_;
    size_t pos_;
};

class OutputStringStream : public OutputStream {
   public:
    OutputStringStream();
    void write(const char* buffer, size_t toWrite) override;
    void flush() override;

    const string& str() const;

   private:
    string s_;
};

static constexpr size_t DEFAULT_BUFFER_SIZE = 1 << 20;
PInputStream openFileBufferedReader(const string& filename, size_t bufferSize = DEFAULT_BUFFER_SIZE);

template <typename T>
OutputStream& operator<<(OutputStream& stream, const T& x) {
    stream.write(to_string(x));
    return stream;
}

#define OUT_INT_TEMPLATE(TYPENAME) \
 template <> \
 inline OutputStream& operator<<<TYPENAME>(OutputStream& stream, const TYPENAME& x) { \
    thread_local char buffer[64]; \
    const size_t len = numToBuffer<TYPENAME, 10>(x, buffer); \
     stream.write(buffer, len); \
     return stream; \
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
inline OutputStream& operator<<<string>(OutputStream& stream, const string& s) {
    stream.write(s);
    return stream;
}
