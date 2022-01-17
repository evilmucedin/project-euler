#pragma once

#include <memory>

using namespace std;

class InputStream {
public:
    virtual ~InputStream();

    size_t read(char* buffer, size_t toRead) = 0;
};

class OutputStream {
public:
    virtual ~OutputStream();

    size_t write(const char* buffer, size_t toWrite) = 0;
};

class StdInputStream : public InputStream {
public:
    size_t read(char* buffer, size_t toRead) override;
};

class StdOutputStream : public OutputStream {
public:
    size_t write(const char* buffer, size_t toWrite) override;
};

class BufferedInputStream : public InputStream {
public:
    size_t read(char* buffer, size_t toRead) override;
};

class BufferedOutputStream : public OutputStream {
public:
    size_t write(const char* buffer, size_t toWrite) override;
};
