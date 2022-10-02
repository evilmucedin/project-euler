#include "file.h"

#include <cstdio>

#include "lib/exception.h"

#ifdef __APPLE__
#define fread_unlocked fread
#define fwrite_unlocked fwrite
#endif

File::File(string filename, string mode) : filename_(std::move(filename)), mode_(std::move(mode)) {
    f_ = fopen(filename_.c_str(), mode_.c_str());
    if (!f_) {
        THROW("Could not open '" << filename_ << "'");
    }
}

File::~File() { close(); }

void File::write(const char* buffer, size_t size) {
    if (fwrite_unlocked(buffer, 1, size, f_) != size) {
        THROW("Write failed");
    }
}

size_t File::read(char* buffer, size_t size) {
    size_t read = fread_unlocked(buffer, 1, size, f_);
    if (read == 0) {
        if (auto error = ferror(f_)) {
            perror("Read failed: ");
            THROW("Read failed read=" << read << " size=" << size << " err=" << error << " filename=" << filename_);
        }
        return 0;
    }
    return read;
}

uint64_t File::tell() {
    auto res = ftell(f_);
    if (res == -1L) {
        THROW("ftell failed");
    }
    return res;
}

void File::seek(uint64_t offset) {
    if (fseek(f_, offset, SEEK_SET) != 0) {
        THROW("Seek failed");
    }
}

void File::seekEnd(int64_t offset) {
    if (fseek(f_, offset, SEEK_END) != 0) {
        THROW("Seek failed");
    }
}

void File::setvbuf(size_t size) { std::setvbuf(f_, nullptr, _IOFBF, size); }

void File::flush() {
    if (fflush(f_)) {
        THROW("flush failed");
    }
}

void File::close() {
    if (fclose(f_)) {
        THROW("fclose failed '" << filename_ << "'");
        f_ = nullptr;
    }
}

bool File::opened() const { return f_ != nullptr; }

size_t File::printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto res = fprintf(f_, fmt, args);
    va_end(args);
    return res;
}
