#include "lib/io/fstream.h"

#include <array>
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

string modeToString(ios_base::openmode mode) {
    static constexpr array<ios_base::openmode, 6> kModeValV{ios_base::in,  ios_base::out,   ios_base::app,
                                                            ios_base::ate, ios_base::trunc, ios_base::binary};

    static const array<string, kModeValV.size()> kModeNameV{"in", "out", "app", "ate", "trunc", "binary"};

    string res;
    for (size_t i = 0; i < kModeValV.size(); ++i) {
        if (mode & kModeValV[i]) {
            res += (!res.empty()) ? "|" : "";
            res += kModeNameV[i];
        }
    }
    if (res.empty()) {
        res = "none";
    }
    return res;
}

void checkMode(const string& filename, ios_base::openmode mode) {
    if ((mode & ios_base::trunc) && !(mode & ios_base::out)) {
        throw Exception(string("strict_fstream: open('") + filename + "'): mode error: trunc and not out");
    } else if ((mode & ios_base::app) && !(mode & ios_base::out)) {
        throw Exception(string("strict_fstream: open('") + filename + "'): mode error: app and not out");
    } else if ((mode & ios_base::trunc) && (mode & ios_base::app)) {
        throw Exception(string("strict_fstream: open('") + filename + "'): mode error: trunc and app");
    }
}

void checkOpen(ios* s_p, const string& filename, ios_base::openmode mode) {
    if (s_p->fail()) {
        throw Exception(string("strict_fstream: open('") + filename + "'," + modeToString(mode) +
                        "): open failed: " + strError());
    }
}

void checkPeek(istream* is_p, const string& filename, ios_base::openmode mode) {
    bool peekFailed = true;
    try {
        is_p->peek();
        peekFailed = is_p->fail();
    } catch (const ios_base::failure& e) {
    }
    if (peekFailed) {
        throw Exception(string("strict_fstream: open('") + filename + "'," + modeToString(mode) +
                        "): peek failed: " + strError());
    }
    is_p->clear();
}

}  // namespace

IFStream::IFStream(const string& filename, ios_base::openmode mode) { open(filename, mode); }

void IFStream::open(const string& filename, ios_base::openmode mode) {
    mode |= ios_base::in;
    exceptions(ios_base::badbit);
    checkMode(filename, mode);
    ifstream::open(filename, mode);
    checkOpen(this, filename, mode);
    checkPeek(this, filename, mode);
}

bool IFStream::readLine(string& line) { return (bool)std::getline(*this, line); }

string IFStream::readLine() {
    string line;
    if (!readLine(line)) {
        throw Exception("Read failed");
    }
    return line;
}

OFStream::OFStream(const string& filename, ios_base::openmode mode) { open(filename, mode); }

void OFStream::open(const string& filename, ios_base::openmode mode) {
    mode |= ios_base::out;
    exceptions(ios_base::badbit);
    checkMode(filename, mode);
    ofstream::open(filename, mode);
    checkOpen(this, filename, mode);
}
