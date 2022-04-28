#include "stream.h"

#include <fcntl.h>
#include <lib/exception.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>

namespace {

string strError() {
    string buff(80, '\0');
    auto p = strerror_r(errno, const_cast<char*>(buff.data()), buff.size());
#ifndef __APPLE__
    return string(p, strlen(p));
#else
    return buff;
#endif
}

}  // namespace

InputStream::~InputStream() {}

bool InputStream::readChar(char& ch) { return read(&ch, 1) > 0; }

bool InputStream::readTo(string& s, char to) {
    s.clear();

    char ch;

    if (!read(&ch, 1)) {
        return false;
    }

    do {
        if (ch == to) {
            break;
        }

        s += ch;
    } while (read(&ch, 1));

    return true;
}

bool InputStream::readToken(string& s) {
    s.clear();

    char ch;
    if (!read(&ch, 1)) {
        return false;
    }

    while (isspace(ch) && read(&ch, 1)) {
    }

    do {
        if (isspace(ch)) {
            break;
        }

        s += ch;
    } while (read(&ch, 1));

    return true;
}

bool InputStream::readLine(string& s) { return readTo(s, '\n'); }

string InputStream::readLine() {
    string result;
    readLine(result);
    return result;
}

void OutputStream::write(const string& s) { write(s.data(), s.size()); }

void OutputStream::write(char ch) { write(&ch, 1); }

OutputStream::~OutputStream() {}

void Endl(OutputStream& o) { (o << "\n").flush(); }

void StdOutputStream::write(const char* buffer, size_t toWrite) {
    const auto res = ::write(1, buffer, toWrite);
    if (res != toWrite) {
        THROW("write to stdout failed '" << strError() << "'");
    }
}

void StdOutputStream::flush() {}

FileInputStream::FileInputStream(const string& filename) : filename_(filename), fd_(-1), eof_(false) {
    fd_ = open(filename_.c_str(), O_RDONLY);
    if (-1 == fd_) {
        THROW("failed to open file '" << strError() << "'");
    }
}

FileInputStream::~FileInputStream() {
    if (-1 != fd_) {
        close(fd_);
        fd_ = -1;
    }
}

size_t FileInputStream::read(char* buffer, size_t toRead) {
    if (toRead) {
        const auto res = ::read(fd_, buffer, toRead);
        if (res == 0) {
            eof_ = true;
        }
        return res;
    } else {
        return 0;
    }
}

bool FileInputStream::eof() const { return eof_; }

FileOutputStream::FileOutputStream(const string& filename) : filename_(filename) {
    fd_ = open(filename_.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (-1 == fd_) {
        throw Exception("failed to open file");
    }
}

FileOutputStream::~FileOutputStream() {
    if (-1 != fd_) {
        close(fd_);
        fd_ = -1;
    }
}

void FileOutputStream::write(const char* buffer, size_t toWrite) {
    if (toWrite) {
        const auto written = ::write(fd_, buffer, toWrite);
        if (toWrite != written) {
            THROW("write failed " << written << " " << toWrite << " " << errno);
        }
    }
}

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

bool BufferedInputStream::eof() const {
    if (bufferPos_ == bufferEnd_) {
        return nested_->eof();
    }
    return false;
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

InputStringStream::InputStringStream(string s) : s_(std::move(s)), pos_(0) {}

size_t InputStringStream::read(char* buffer, size_t toRead) {
    const size_t res = std::min(toRead, s_.size() - pos_);
    memcpy(buffer, s_.c_str() + pos_, res);
    pos_ += res;
    return res;
}

bool InputStringStream::eof() const { return pos_ == s_.size(); }

OutputStringStream::OutputStringStream() {}

void OutputStringStream::write(const char* buffer, size_t toWrite) { s_.append(buffer, toWrite); }

void OutputStringStream::flush() {}

const string& OutputStringStream::str() const { return s_; }

PInputStream openFileBufferedReader(const string& filename, size_t bufferSize) {
    auto fReader = make_shared<FileInputStream>(filename);
    return make_shared<BufferedInputStream>(fReader, bufferSize);
}
