#pragma once

#include <memory>

using namespace std;

class InputStream {
   public:
    virtual ~InputStream();

    size_t read(char* buffer, size_t toRead) = 0;
};

using PInputStream = std::shared_ptr<InputStream>;

class OutputStream {
   public:
    virtual ~OutputStream();

    size_t write(const char* buffer, size_t toWrite) = 0;
    void flush() = 0;
};

using POutputStream = std::shared_ptr<OutputStream>;

class StdInputStream : public InputStream {
   public:
    size_t read(char* buffer, size_t toRead) override;
};

class StdOutputStream : public OutputStream {
   public:
    size_t write(const char* buffer, size_t toWrite) override;
    void flush() override;
};

class BufferedInputStream : public InputStream {
   public:
    BufferedInputStream(PInputStream nested);
    size_t read(char* buffer, size_t toRead) override;

   private:
    PInputStream nested_;
};

class BufferedOutputStream : public OutputStream {
   public:
    static constexpr size_t DEFAULT_BUFFER_SIZE = 1 << 20;
    BufferedOutputStream(POutputStream nested, size_t bufferSize = DEFAULT_BUFFER_SIZE);

    size_t write(const char* buffer, size_t toWrite) override;
    void flush() override;

   private:
    POutputStream nested_;
    size_t bufferSize_;
    unique_ptr<char[]> buffer_;
    size_t bufferPos_;
};
