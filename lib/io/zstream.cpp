#include "lib/io/zstream.h"

ZIStream::ZIStream(shared_ptr<istream> stream) : istream(new ZIStreamBuf(stream->rdbuf())), stream_(stream) {}

ZIStream::~ZIStream() {
    delete rdbuf();
}
