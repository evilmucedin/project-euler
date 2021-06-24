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

WChar File::getWC() { return fgetwc(f_); }

int File::getC() {
    return fgetc(f_);
}

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
