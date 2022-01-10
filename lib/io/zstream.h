#pragma once

#include "lib/header.h"
#include "zlib/zlib.h"

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

class ZIStreamBuf : public streambuf {
   public:
    ZIStreamBuf(streambuf* pSBuf, size_t buffSize = kDefaultBuffSize);
    ZIStreamBuf(const ZIStreamBuf&) = delete;
    ZIStreamBuf(ZIStreamBuf&&) = default;
    ZIStreamBuf& operator=(const ZIStreamBuf&) = delete;
    ZIStreamBuf& operator=(ZIStreamBuf&&) = default;
    virtual ~ZIStreamBuf() = default;

    streambuf::int_type underflow() override;

   private:
    streambuf* pSBuf_;
    vector<char> inBuff_;
    char* inBuffStart_;
    char* inBuffEnd_;
    vector<char> outBuff_;
    unique_ptr<ZStreamWrapper> zStrm_;
    size_t buffSize_;

    static constexpr size_t kDefaultBuffSize = 1 << 20;
};

class ZIStream : public istream {
   public:
    ZIStream(shared_ptr<istream> stream);
    virtual ~ZIStream();

   private:
    shared_ptr<istream> stream_;
};

class ZOStreamBuf : public streambuf {
   public:
    ZOStreamBuf(streambuf* pSBuf, size_t buffSize = kDefaultBuffSize);
    ZOStreamBuf(const ZOStreamBuf&) = delete;
    ZOStreamBuf(ZOStreamBuf&&) = default;
    ZOStreamBuf& operator=(const ZOStreamBuf&) = delete;
    ZOStreamBuf& operator=(ZOStreamBuf&&) = default;
    virtual ~ZOStreamBuf();

    std::streamsize zflush(bool flush);

   protected:
    int sync();
    int_type overflow(int_type c);
    std::streamsize flush();

   private:
    streambuf* pSBuf_;
    vector<char> inBuff_;
    char* inBuffStart_;
    char* inBuffEnd_;
    vector<char> outBuff_;
    unique_ptr<ZStreamWrapper> zStrm_;
    size_t buffSize_;

    static constexpr size_t kDefaultBuffSize = 1 << 20;
};

class ZOStream : public ostream {
   public:
    ZOStream(shared_ptr<ostream> stream);
    std::streamsize flush();
    virtual ~ZOStream();

   private:
    ZOStreamBuf* buf_;
    shared_ptr<ostream> stream_;
};
