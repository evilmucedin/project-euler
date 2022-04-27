#pragma once

#include "zlib/zlib.h"

#include "lib/header.h"
#include "lib/io/stream.h"

class ZlibException : public Exception {
   public:
    ZlibException(int ret);
    ZlibException(string msg);
};

class ZlibStreamWrapper : public z_stream {
   public:
    ZlibStreamWrapper(bool isInput = true, int level = Z_DEFAULT_COMPRESSION);
    ~ZlibStreamWrapper();

   private:
    bool isInput_;
};

class ZlibInputStream : public InputStream {
   public:
    ZlibInputStream(PInputStream nested, size_t buffSize = kDefaultBuffSize);
    ZlibInputStream(const ZlibInputStream&) = delete;
    ZlibInputStream(ZlibInputStream&&) = default;
    ZlibInputStream& operator=(const ZlibInputStream&) = delete;
    ZlibInputStream& operator=(ZlibInputStream&&) = default;

    size_t read(char* buffer, size_t toRead) override;

   private:
    size_t readSome(char* buffer, size_t toRead);

    PInputStream nested_;
    vector<char> inBuff_;
    char* inBuffStart_;
    char* inBuffEnd_;
    vector<char> outBuff_;
    char* outBuffFreeStart_;
    char* outBuffNext_;
    unique_ptr<ZlibStreamWrapper> zStrm_;
    size_t buffSize_;
    bool zeof_;

    static constexpr size_t kDefaultBuffSize = 1 << 20;
};

class ZlibOutputStream : public OutputStream {
   public:
    ZlibOutputStream(POutputStream nested, size_t buffSize = kDefaultBuffSize);
    ZlibOutputStream(const ZlibOutputStream&) = delete;
    ZlibOutputStream(ZlibOutputStream&&) = default;
    ZlibOutputStream& operator=(const ZlibOutputStream&) = delete;
    ZlibOutputStream& operator=(ZlibOutputStream&&) = default;
    virtual ~ZlibOutputStream();

    void flush() override;
    void write(const char* buffer, size_t toWrite) override;

   private:
    void zflush(bool flush);

    POutputStream nested_;
    vector<char> inBuff_;
    char* inBuffStart_;
    char* inBuffEnd_;
    vector<char> outBuff_;
    char* outBuffStart_;
    char* outBuffEnd_;
    unique_ptr<ZlibStreamWrapper> zStrm_;
    size_t buffSize_;

    static constexpr size_t kDefaultBuffSize = 1 << 20;
};
