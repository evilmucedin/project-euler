#include "stream.h"

#include <unistd.h>

#include <cstring>

#include <lib/exception.h>

InputStream::~InputStream() {}

void OutputStream::write(char ch) { write(&ch, 1); }

OutputStream::~OutputStream() {}

void StdOutputStream::write(const char* buffer, size_t toWrite) {
    const auto res = ::write(1, buffer, toWrite);
    if (res != toWrite) {
        throw Exception("write to stdout failed");
    }
}

void StdOutputStream::flush() {}

FileOutputStream::FileOutputStream(const string& filename) : filename_(filename), file_(filename, "wb") {}

void FileOutputStream::write(const char* buffer, size_t toWrite) { file_.write(buffer, toWrite); }

void FileOutputStream::flush() {}

BufferedInputStream::BufferedInputStream(PInputStream nested, size_t bufferSize)
    : nested_(nested), bufferSize_(bufferSize), buffer_(new char[bufferSize_]), bufferPos_(0), bufferEnd_(0) {}

BufferedInputStream::~BufferedInputStream() {}

size_t BufferedInputStream::read(char* buffer, size_t toRead) {
    size_t read = 0;
    if (bufferPos_ == bufferEnd_) {
        refill();
    }
    while (toRead && bufferEnd_) {
        if (bufferPos_ == bufferEnd_) {
            refill();
        }
        const size_t canRead = std::min(toRead, bufferEnd_ - bufferPos_);
        memcpy(buffer, buffer_.get() + bufferPos_, canRead);
        bufferPos_ += canRead;
        toRead -= canRead;
        read += canRead;
    }
    return read;
}

void BufferedInputStream::refill() {
    ASSERTEQ(bufferPos_, bufferEnd_);
    bufferPos_ = 0;
    bufferEnd_ = nested_->read(buffer_.get(), bufferSize_);
}

BufferedOutputStream::BufferedOutputStream(POutputStream nested, size_t bufferSize)
    : nested_(nested), bufferSize_(bufferSize), buffer_(new char[bufferSize_]), bufferPos_(0) {}

BufferedOutputStream::~BufferedOutputStream() { flush(); }

void BufferedOutputStream::write(const char* buffer, size_t toWrite) {
    while (toWrite) {
        const size_t canWrite = std::min(toWrite, bufferSize_ - bufferPos_);
        memcpy(buffer_.get() + bufferPos_, buffer, canWrite);
        toWrite -= canWrite;
        buffer += canWrite;
        bufferPos_ += canWrite;
        if (bufferPos_ == bufferSize_) {
            flush();
        }
    }
}

void BufferedOutputStream::flush() {
    if (bufferPos_) {
        nested_->write(buffer_.get(), bufferPos_);
        bufferPos_ = 0;
    }
}
