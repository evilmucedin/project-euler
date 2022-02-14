#include "stream.h"

#include <unistd.h>

#include <cstring>

#include <lib/exception.h>

InputStream::~InputStream() {}

OutputStream::~OutputStream() {}

size_t StdOutputStream::write(const char* buffer, size_t toWrite) {
    const auto res = ::write(1, buffer, toWrite);
    if (res != toWrite) {
        throw Exception("write to stdout failed");
    }
    return res;
}

void StdOutputStream::flush() {}

BufferedInputStream::BufferedInputStream(PInputStream nested) : nested_(nested) {}

BufferedInputStream::~BufferedInputStream() {}

size_t BufferedInputStream::read(char* buffer, size_t toRead) { return nested_->read(buffer, toRead); }

BufferedOutputStream::BufferedOutputStream(POutputStream nested, size_t bufferSize)
    : nested_(nested), bufferSize_(bufferSize), buffer_(new char[bufferSize_]), bufferPos_(0) {}

BufferedOutputStream::~BufferedOutputStream() { flush(); }

size_t BufferedOutputStream::write(const char* buffer, size_t toWrite) {
    size_t written = 0;
    while (toWrite) {
        size_t canWrite = std::min(toWrite, bufferSize_ - bufferPos_);
        memcpy(buffer_.get() + bufferPos_, buffer, canWrite);
        toWrite -= canWrite;
        buffer += canWrite;
        written += canWrite;
        if (bufferPos_ == bufferSize_) {
            flush();
        }
    }
    return written;
}

void BufferedOutputStream::flush() {
    if (bufferPos_) {
        nested_->write(buffer_.get(), bufferPos_);
        bufferPos_ = 0;
    }
}
