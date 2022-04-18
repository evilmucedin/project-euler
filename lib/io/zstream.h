#pragma once

#include "zlib/zlib.h"

#include "lib/header.h"
#include "lib/io/stream.h"

class ZException : public Exception {
   public:
    ZException(int ret);
    ZException(string msg);
};

class ZStreamWrapper : public z_stream {
   public:
    ZStreamWrapper(bool isInput = true, int level = Z_DEFAULT_COMPRESSION);
    ~ZStreamWrapper();

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
    PInputStream nested_;
    vector<char> inBuff_;
    char* inBuffStart_;
    char* inBuffEnd_;
    vector<char> outBuff_;
    unique_ptr<ZStreamWrapper> zStrm_;
    size_t buffSize_;

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

   protected:
    int sync();
    int_type overflow(int_type c);
    std::streamsize flush();

   private:
    POutputStream nested_;
    vector<char> inBuff_;
    char* inBuffStart_;
    char* inBuffEnd_;
    vector<char> outBuff_;
    unique_ptr<ZStreamWrapper> zStrm_;
    size_t buffSize_;

    static constexpr size_t kDefaultBuffSize = 1 << 20;
};
