#include "lib/io/zstreamDeprecated.h"

#include <cassert>

ZException::ZException(int ret) : Exception("zlib: ") {
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

ZException::ZException(string msg) : Exception(msg) {}

ZStreamWrapper::ZStreamWrapper(bool isInput, int level) : isInput_(isInput) {
    zalloc = Z_NULL;
    zfree = Z_NULL;
    opaque = Z_NULL;
    int ret;
    if (isInput_) {
        avail_in = 0;
        next_in = Z_NULL;
        ret = inflateInit2(this, 15 + 32);
    } else {
        ret = deflateInit2(this, level, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
    }
    if (ret != Z_OK) {
        throw ZException(ret);
    }
}

ZStreamWrapper::~ZStreamWrapper() {
    if (isInput_) {
        inflateEnd(this);
    } else {
        deflateEnd(this);
    }
}

ZIStreamBuf::ZIStreamBuf(streambuf* pSBuf, size_t buffSize) : pSBuf_(pSBuf), buffSize_(buffSize) {
    assert(pSBuf_);
    inBuff_.resize(buffSize_);
    inBuffStart_ = inBuff_.data();
    inBuffEnd_ = inBuff_.data();
    outBuff_.resize(buffSize_);
    setg(outBuff_.data(), outBuff_.data(), outBuff_.data());
}

streambuf::int_type ZIStreamBuf::underflow() {
    if (gptr() == egptr()) {
        char* outBuffFreeStart = outBuff_.data();
        do {
            if (inBuffStart_ == inBuffEnd_) {
                inBuffStart_ = inBuff_.data();
                auto sz = pSBuf_->sgetn(inBuff_.data(), buffSize_);
                inBuffEnd_ = inBuff_.data() + sz;
                if (inBuffEnd_ == inBuffStart_) {
                    break;
                }
            }
            if (!zStrm_) {
                zStrm_ = make_unique<ZStreamWrapper>(true);
            }
            zStrm_->next_in = reinterpret_cast<Bytef*>(inBuffStart_);
            zStrm_->avail_in = inBuffEnd_ - inBuffStart_;
            zStrm_->next_out = reinterpret_cast<Bytef*>(outBuffFreeStart);
            zStrm_->avail_out = (outBuff_.data() + buffSize_) - outBuffFreeStart;
            auto ret = inflate(zStrm_.get(), Z_NO_FLUSH);
            if (ret != Z_OK && ret != Z_STREAM_END) {
                throw ZException(ret);
            }
            inBuffStart_ = reinterpret_cast<char*>(zStrm_->next_in);
            ASSERTEQ(inBuffEnd_, inBuffStart_ + zStrm_->avail_in);
            outBuffFreeStart = reinterpret_cast<char*>(zStrm_->next_out);
            ASSERTEQ(outBuffFreeStart + zStrm_->avail_out, outBuff_.data() + buffSize_);
            if (ret == Z_STREAM_END) {
                zStrm_.reset();
            }
        } while (outBuffFreeStart == outBuff_.data());
        setg(outBuff_.data(), outBuff_.data(), outBuffFreeStart);
    }
    return (gptr() == egptr()) ? traits_type::eof() : traits_type::to_int_type(*gptr());
}

ZIStream::ZIStream(shared_ptr<istream> stream) : istream(new ZIStreamBuf(stream->rdbuf())), stream_(stream) {}

ZIStream::~ZIStream() {
    delete rdbuf();
}

ZOStreamBuf::ZOStreamBuf(streambuf* pSBuf, size_t buffSize) : pSBuf_(pSBuf), buffSize_(buffSize) {
    assert(pSBuf_);
    inBuff_.resize(buffSize_);
    inBuffStart_ = inBuff_.data();
    inBuffEnd_ = inBuff_.data();
    outBuff_.resize(buffSize_);
    setp(outBuff_.data(), outBuff_.data() + buffSize_);
}

ZOStreamBuf::~ZOStreamBuf() {
    zflush(true);
}

int ZOStreamBuf::sync() {
    if (pptr() && pptr() > pbase()) {
        int c = overflow(EOF);
        if (c == EOF) {
            return -1;
        }
    }

    return 0;
}

std::streamsize ZOStreamBuf::zflush(bool flush) {
    cerr << "zflush" << endl;
    std::streamsize totalWritten = 0;
    if (!zStrm_) {
        zStrm_ = make_unique<ZStreamWrapper>(false);
    }
    zStrm_->next_in = reinterpret_cast<Bytef*>(outBuff_.data());
    zStrm_->avail_in = pptr() - outBuff_.data();
    zStrm_->next_out = reinterpret_cast<Bytef*>(inBuff_.data());
    zStrm_->avail_out = buffSize_;

    int err = Z_OK;
    do {
        err = deflate(zStrm_.get(), (flush) ? Z_FINISH : 0);
        const auto written = buffSize_ - zStrm_->avail_out;
        totalWritten += written;
        if (err == Z_OK || err == Z_STREAM_END) {
            pSBuf_->sputn(inBuff_.data(), written);
        } else {
            throw Exception("compression error");
        }
    } while (zStrm_->avail_in != 0 && err == Z_OK);
    ASSERTEQ(zStrm_->avail_in, 0);

    setp(outBuff_.data(), outBuff_.data() + buffSize_);
    return totalWritten;
}

ZOStreamBuf::int_type ZOStreamBuf::overflow(int_type c) {
    if (c != EOF) {
        if (pptr() == outBuff_.data() + buffSize_) {
            zflush(false);
        }
        ASSERTLT(pptr(), outBuff_.data() + buffSize_);

        *pptr() = c;
        pbump(1);
    }
    return c;
}

ZOStream::ZOStream(shared_ptr<ostream> stream)
    : ostream(new ZOStreamBuf(stream->rdbuf())), buf_(reinterpret_cast<ZOStreamBuf*>(rdbuf())), stream_(stream) {}

ZOStream::~ZOStream() { delete buf_; }

std::streamsize ZOStream::flush() {
    const auto res = buf_->zflush(true);
    stream_->flush();
    return res;
}

