#include "lib/io/zstream.h"

#include <cassert>
#include <cstring>

ZlibException::ZlibException(int ret) : Exception("zlib: ") {
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

ZlibException::ZlibException(string msg) : Exception(msg) {}

ZlibStreamWrapper::ZlibStreamWrapper(bool isInput, int level) : isInput_(isInput) {
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
        throw ZlibException(ret);
    }
}

ZlibStreamWrapper::~ZlibStreamWrapper() {
    if (isInput_) {
        inflateEnd(this);
    } else {
        deflateEnd(this);
    }
}

ZlibInputStream::ZlibInputStream(PInputStream nested, size_t buffSize)
    : nested_(nested), buffSize_(buffSize), zeof_(false) {
    assert(nested_.get());
    assert(buffSize);
    inBuff_.resize(buffSize_);
    inBuffStart_ = inBuff_.data();
    inBuffEnd_ = inBuff_.data();
    outBuff_.resize(buffSize_);
    outBuffFreeStart_ = outBuff_.data();
    outBuffNext_ = outBuffFreeStart_;
}

size_t ZlibInputStream::readSome(char* buffer, size_t toRead) {
    if (outBuffNext_ == outBuffFreeStart_) {
        if (!zeof_) {
            if (outBuffFreeStart_ == outBuff_.data() + buffSize_) {
                outBuffFreeStart_ = outBuff_.data();
                outBuffNext_ = outBuff_.data();
            }

            do {
                if (inBuffStart_ == inBuffEnd_) {
                    inBuffStart_ = inBuff_.data();
                    auto sz = nested_->read(inBuff_.data(), buffSize_);
                    if (sz == 0) {
                        break;
                    }
                    inBuffEnd_ = inBuff_.data() + sz;
                    if (inBuffEnd_ == inBuffStart_) {
                        break;
                    }
                }
                if (!zStrm_) {
                    zStrm_ = make_unique<ZlibStreamWrapper>(true);
                }
                zStrm_->next_in = reinterpret_cast<Bytef*>(inBuffStart_);
                zStrm_->avail_in = inBuffEnd_ - inBuffStart_;
                zStrm_->next_out = reinterpret_cast<Bytef*>(outBuffFreeStart_);
                zStrm_->avail_out = (outBuff_.data() + buffSize_) - outBuffFreeStart_;
                auto ret = inflate(zStrm_.get(), Z_NO_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    throw ZlibException(ret);
                }
                inBuffStart_ = reinterpret_cast<char*>(zStrm_->next_in);
                ASSERTEQ(inBuffEnd_, inBuffStart_ + zStrm_->avail_in);
                outBuffFreeStart_ = reinterpret_cast<char*>(zStrm_->next_out);
                ASSERTEQ(outBuffFreeStart_ + zStrm_->avail_out, outBuff_.data() + buffSize_);
                if (ret == Z_STREAM_END) {
                    zStrm_.reset();
                    zeof_ = true;
                }
            } while (outBuffFreeStart_ == outBuff_.data());
        }
    }

    const size_t readNow = std::min<size_t>(toRead, outBuffFreeStart_ - outBuffNext_);
    memcpy(buffer, outBuffNext_, readNow);
    outBuffNext_ += readNow;
    return readNow;
}

size_t ZlibInputStream::read(char* buffer, size_t toRead) {
    size_t result = 0;
    while (toRead) {
        const size_t readResult = readSome(buffer, toRead);
        if (readResult) {
            result += readResult;
            toRead -= readResult;
            buffer += readResult;
        } else {
            break;
        }
    }
    return result;
}

bool ZlibInputStream::eof() const {
    if (outBuffFreeStart_ == outBuffNext_) {
        return zeof_;
    }
    return false;
}

ZlibOutputStream::ZlibOutputStream(POutputStream nested, size_t buffSize) : nested_(nested), buffSize_(buffSize) {
    assert(nested_.get());
    inBuff_.resize(buffSize_);
    inBuffStart_ = inBuff_.data();
    inBuffEnd_ = inBuff_.data();
    outBuff_.resize(buffSize_);
    outBuffStart_ = outBuff_.data();
    outBuffEnd_ = outBuff_.data() + buffSize_;
}

ZlibOutputStream::~ZlibOutputStream() { zflush(true); }

void ZlibOutputStream::flush() { zflush(false); }

void ZlibOutputStream::write(const char* buffer, size_t toWrite) {
    while (toWrite) {
        const size_t next = std::min<size_t>(toWrite, outBuffEnd_ - outBuffStart_);
        memcpy(outBuffStart_, buffer, next);
        outBuffStart_ += next;
        toWrite -= next;
        if (outBuffStart_ == outBuffEnd_) {
            zflush(false);
        }
    }
}

void ZlibOutputStream::zflush(bool flush) {
    std::streamsize totalWritten = 0;
    if (!zStrm_) {
        zStrm_ = make_unique<ZlibStreamWrapper>(false);
    }
    zStrm_->next_in = reinterpret_cast<Bytef*>(outBuff_.data());
    zStrm_->avail_in = outBuffStart_ - outBuff_.data();
    zStrm_->next_out = reinterpret_cast<Bytef*>(inBuff_.data());
    zStrm_->avail_out = buffSize_;

    int err = Z_OK;
    do {
        err = deflate(zStrm_.get(), (flush) ? Z_FINISH : 0);
        const auto written = buffSize_ - zStrm_->avail_out;
        totalWritten += written;
        if (err == Z_OK || err == Z_STREAM_END) {
            nested_->write(inBuff_.data(), written);
        } else {
            throw Exception("compression error");
        }
    } while (zStrm_->avail_in != 0 && err == Z_OK);
    ASSERTEQ(zStrm_->avail_in, 0);

    outBuffStart_ = outBuff_.data();
}

PInputStream openZlibFileBufferedReader(const string& filename, size_t bufferSize) {
    return make_shared<ZlibInputStream>(openFileBufferedReader(filename, bufferSize), bufferSize);
}

POutputStream openZlibFileBufferedWriter(const string& filename, size_t bufferSize) {
    return make_shared<ZlibOutputStream>(openFileBufferedWriter(filename, bufferSize), bufferSize);
}
