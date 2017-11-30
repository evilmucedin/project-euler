#include "lib/io/csv.h"

#include <cassert>

CsvParser::CsvParser(shared_ptr<istream> stream, char delim, char quote) : stream_(move(stream)), delim_(delim), quote_(quote) {}

bool CsvParser::readHeader() {
    if (!readLine()) {
        return false;
    }
    header_ = line_;
    for (size_t i = 0; i < header_.size(); ++i) {
        assert(fieldToIndex_.count(header_[i]) == 0);
        fieldToIndex_.emplace(header_[i], i);
    }
    return true;
}

bool CsvParser::readLine() {
    if (!getline(*stream_, sLine_)) {
        return false;
    }
    line_.clear();
    bool inQuote = false;
    const char* begin = sLine_.data();
    const char* p = begin;
    while (*p) {
        if (inQuote) {
            if (*p == quote_) {
                inQuote = false;
            }
        } else {
            if (*p == delim_) {
                line_.emplace_back(begin, p - begin);
                begin = p + 1;
            } else if (*p == quote_) {
                inQuote = true;
            }
        }
        ++p;
    }
    line_.emplace_back(begin, p - begin);
    for (auto& s: line_) {
        unquote(s);
    }
    return true;
}

size_t CsvParser::size() const { return line_.size(); }

const string& CsvParser::get(size_t index) const {
    assert(index < line_.size());
    return line_[index];
}

int CsvParser::getIndex(const string& s) const {
    return findWithDefault(fieldToIndex_, s, -1);
}

void CsvParser::unquote(string& s) {
    const char* r = s.data();
    if (*r != quote_) {
        return;
    }
    char* w = const_cast<char*>(r);
    while (*r) {
        if (*r == quote_) {
            if (r[1] == quote_) {
                *w++ = '\"';
                r += 2;
            } else {
                ++r;
            }
        } else {
            *w++ = *r++;
        }
    }
    s.resize(w - s.data());
}

