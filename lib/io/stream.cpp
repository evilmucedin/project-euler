#include "stream.h"

InputStream::~InputStream() {}

OutputStream::~OutputStream() {}

    size_t StdOutputStream::write(const char* buffer, size_t toWrite) {
        const auto res = write(1, buffer, toWrite);
        if (res != toWrite) {
            throw Exception("write to stdout failed");
        }
    }

    void StdOutputStream::flush() {
    }

BufferedInputStream::BufferedInputStream(PInputStream nested) : nested_(nested) {}

BufferedOutputStream::BufferedOutputStream(POutputStream nested, size_t bufferSize)
    : nested_(nested), bufferSize_(bufferSize), buffer_(new char[bufferSize_]), bufferPos_(0) {}

BufferedOutputStream::~BufferedOutputStream() {
    flush();
}


size_t BufferedOutputStream::write(const char* buffer, size_t toWrite) {
    while (toWrite) {
        size_t canWrite = std::min(toWrite, bufferSize_ - bufferPos_);
        memcpy(buffer_.get() + bufferPos_, buffer, canWrite);
        toWrite -= canWrite;
        buffer += canWrite;
        if (bufferPos_ == bufferSize_) {
            flush();
        }
    }
}

void BufferedOutputStream::flush() override {
    if (bufferPos_) {
        nested_->write(buffer_.get(), bufferPos_);
        bufferPos_ = 0;
    }
}
