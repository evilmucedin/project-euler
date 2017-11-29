#pragma once

#include "lib/header.h"

#include "zlib/zlib.h"

class ZException : public std::exception {
   public:
    ZException(int ret) : msg_("zlib: ") {
        switch (ret) {
            case Z_STREAM_ERROR:
                msg_ += "Z_STREAM_ERROR: ";
                break;
            case Z_DATA_ERROR:
                msg_ += "Z_DATA_ERROR: ";
                break;
            case Z_MEM_ERROR:
                msg_ += "Z_MEM_ERROR: ";
                break;
            case Z_VERSION_ERROR:
                msg_ += "Z_VERSION_ERROR: ";
                break;
            case Z_BUF_ERROR:
                msg_ += "Z_BUF_ERROR: ";
                break;
            default:
                msg_ += string("[Unknown: ") + to_string(ret) + "]";
                break;
        }
    }
    ZException(const std::string msg) : msg_(msg) {}
    const char* what() const noexcept { return msg_.c_str(); }

   private:
    string msg_;
};

class ZStreamWrapper : public z_stream {
   public:
    ZStreamWrapper(bool _is_input = true, int _level = Z_DEFAULT_COMPRESSION) : is_input(_is_input) {
        zalloc = Z_NULL;
        zfree = Z_NULL;
        opaque = Z_NULL;
        int ret;
        if (is_input) {
            avail_in = 0;
            next_in = Z_NULL;
            ret = inflateInit2(this, 15 + 32);
        } else {
            ret = deflateInit2(this, _level, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
        }
        if (ret != Z_OK) {
            throw ZException(ret);
        }
    }

    ~ZStreamWrapper() {
        if (is_input) {
            inflateEnd(this);
        } else {
            deflateEnd(this);
        }
    }

   private:
    bool is_input;
};

class ZIStreamBuf : public std::streambuf {
   public:
    ZIStreamBuf(std::streambuf* _sbuf_p, std::size_t _buff_size = default_buff_size)
        : sbuf_p(_sbuf_p), buff_size(_buff_size) {
        assert(sbuf_p);
        in_buff.resize(buff_size);
        in_buff_start = in_buff.data();
        in_buff_end = in_buff.data();
        out_buff.resize(buff_size);
        setg(out_buff.data(), out_buff.data(), out_buff.data());
    }

    ZIStreamBuf(const ZIStreamBuf&) = delete;
    ZIStreamBuf(ZIStreamBuf&&) = default;
    ZIStreamBuf& operator=(const ZIStreamBuf&) = delete;
    ZIStreamBuf& operator=(ZIStreamBuf&&) = default;

    virtual ~ZIStreamBuf() = default;

    std::streambuf::int_type underflow() override {
        if (gptr() == egptr()) {
            char* out_buff_free_start = out_buff.data();
            do {
                if (in_buff_start == in_buff_end) {
                    in_buff_start = in_buff.data();
                    std::streamsize sz = sbuf_p->sgetn(in_buff.data(), buff_size);
                    in_buff_end = in_buff.data() + sz;
                    if (in_buff_end == in_buff_start) {
                        break;
                    }
                }
                if (!zstrm_p) {
                    zstrm_p = make_unique<ZStreamWrapper>(true);
                }
                zstrm_p->next_in = reinterpret_cast<decltype(zstrm_p->next_in)>(in_buff_start);
                zstrm_p->avail_in = in_buff_end - in_buff_start;
                zstrm_p->next_out = reinterpret_cast<decltype(zstrm_p->next_out)>(out_buff_free_start);
                zstrm_p->avail_out = (out_buff.data() + buff_size) - out_buff_free_start;
                int ret = inflate(zstrm_p.get(), Z_NO_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    throw ZException(ret);
                }
                in_buff_start = reinterpret_cast<decltype(in_buff_start)>(zstrm_p->next_in);
                in_buff_end = in_buff_start + zstrm_p->avail_in;
                out_buff_free_start = reinterpret_cast<decltype(out_buff_free_start)>(zstrm_p->next_out);
                assert(out_buff_free_start + zstrm_p->avail_out == out_buff.data() + buff_size);
                if (ret == Z_STREAM_END) {
                    zstrm_p.reset();
                }
            } while (out_buff_free_start == out_buff.data());
            setg(out_buff.data(), out_buff.data(), out_buff_free_start);
        }
        return (gptr() == egptr()) ? traits_type::eof() : traits_type::to_int_type(*gptr());
    }

   private:
    std::streambuf* sbuf_p;
    vector<char> in_buff;
    char* in_buff_start;
    char* in_buff_end;
    vector<char> out_buff;
    unique_ptr<ZStreamWrapper> zstrm_p;
    std::size_t buff_size;

    static constexpr std::size_t default_buff_size = (std::size_t)1 << 20;
};

class ZIStream : public istream {
public:
    ZIStream(shared_ptr<istream> stream);
    virtual ~ZIStream();
private:
    shared_ptr<istream> stream_;
};
