#pragma once

#include "lib/header.h"

#include <sstream>

class StreamException : public Exception, public std::ostringstream {
public:
    StreamException();
    StreamException(const StreamException& e);
    StreamException(StreamException&& e);
    StreamException(const string& what);

    const char* what() const noexcept override;

private:
    mutable string s_;
};

#define THROW(...)         \
    {                      \
        StreamException e; \
        e << __VA_ARGS__;  \
        throw e;           \
    }
