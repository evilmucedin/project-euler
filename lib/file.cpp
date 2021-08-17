#include "file.h"

#include "exception.h"

File::File(string filename, string mode) : filename_(std::move(filename)), mode_(std::move(mode)) {
    f_ = fopen(filename_.c_str(), mode_.c_str());
    if (!f_) {
        throw Exception("failed to open file");
    }
}

void File::close() {
    if (f_) {
        fclose(f_);
        f_ = nullptr;
    }
}

void File::write(const char* p, size_t len) {
    if (!len) {
        return;
    }
    if (f_) {
        if (fwrite(p, len, 1, f_) != 1) {
            throw Exception("file write failed");
        }
    } else {
        throw Exception("file is closed");
    }
}

void File::write(const string& s) { write(s.c_str(), s.size()); }

WChar File::getWC() { return fgetwc(f_); }

int File::getC() { return fgetc(f_); }

WChar File::getUTF8C() {
    size_t len;
    unsigned int cp;
    unsigned int min;
    unsigned int buf = getC();
    if (buf == EOF) {
        return WEOF;
    }

    if (buf < 0x80) {
        len = 1;
        min = 0;
        cp = buf;
    } else if (buf < 0xC0) {
        goto error;
    } else if (buf < 0xE0) {
        len = 2;
        min = 1 << 7;
        cp = buf & 0x1F;
    } else if (buf < 0xF0) {
        len = 3;
        min = 1 << (5 + 6);
        cp = buf & 0x0F;
    } else if (buf < 0xF8) {
        len = 4;
        min = 1 << (4 + 6 + 6);
        cp = buf & 0x07;
    } else {
        goto error;
    }

    for (size_t i = 1; i < len; ++i) {
        buf = getC();
        if (buf == EOF) {
            return WEOF;
        }
        if ((buf & 0xC0) != 0x80) {
            goto error;
        }
        cp = (cp << 6) | (buf & 0x3F);
    }

    if (cp < min) {
        goto error;
    }

    if (0xD800 <= cp && cp <= 0xDFFF) {
        goto error;
    }

    if (0x110000 <= cp) {
        goto error;
    }

    return cp;

error:
    return WEOF;
}

bool File::eof() { return feof(f_); }

u64 File::offset() const { return ftell(f_); }

BufferedFileReader::BufferedFileReader(File& f) : f_(f), offset_(0) { tryRead(); }

bool BufferedFileReader::eof() const { return eof_; }

WChar BufferedFileReader::peek() const { return next_; }

WChar BufferedFileReader::advance() {
    const auto result = next_;
    tryRead();
    return result;
}

u64 BufferedFileReader::offset() const {
    return offset_;
}

void BufferedFileReader::tryRead() {
    next_ = f_.getUTF8C();
    eof_ = f_.eof();
    if (!eof_) {
        ++offset_;
    }
}
