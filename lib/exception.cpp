#include "exception.h"

StreamException::StreamException() {}

StreamException::StreamException(const StreamException& e) : ostringstream(e.str()), s_(e.s_) {}

StreamException::StreamException(StreamException&& e) : ostringstream(e.str()), s_(std::move(e.s_)) {}

StreamException::StreamException(const string& what) { *this << what; }

const char* StreamException::what() const noexcept {
    s_ = str();
    return s_.c_str();
}
