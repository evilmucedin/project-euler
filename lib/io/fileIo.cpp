#include "fileIo.h"

#include <cstdio>
#include <cstdarg>

#include "lib/exception.h"

#ifdef __APPLE__
#define fread_unlocked fread
#define fwrite_unlocked fwrite
#endif

FileIo::FileIo(string filename, string mode) : filename_(std::move(filename)), mode_(std::move(mode)) {
    f_ = fopen(filename_.c_str(), mode_.c_str());
    if (!f_) {
        THROW("Could not open '" << filename_ << "'");
    }
}

FileIo::~FileIo() { close(); }

void FileIo::write(const char* buffer, size_t size) {
    if (fwrite_unlocked(buffer, 1, size, f_) != size) {
        THROW("Write failed");
    }
}

size_t FileIo::read(char* buffer, size_t size) {
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

uint64_t FileIo::tell() {
    auto res = ftell(f_);
    if (res == -1L) {
        THROW("ftell failed");
    }
    return res;
}

void FileIo::seek(uint64_t offset) {
    if (fseek(f_, offset, SEEK_SET) != 0) {
        THROW("Seek failed");
    }
}

void FileIo::seekEnd(int64_t offset) {
    if (fseek(f_, offset, SEEK_END) != 0) {
        THROW("Seek failed");
    }
}

void FileIo::setvbuf(size_t size) { std::setvbuf(f_, nullptr, _IOFBF, size); }

void FileIo::flush() {
    if (fflush(f_)) {
        THROW("flush failed");
    }
}

void FileIo::close() {
    if (fclose(f_)) {
        THROW("fclose failed '" << filename_ << "'");
        f_ = nullptr;
    }
}

bool FileIo::opened() const { return f_ != nullptr; }

size_t FileIo::printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto res = fprintf(f_, fmt, args);
    va_end(args);
    return res;
}
