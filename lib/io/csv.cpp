#include "lib/io/csv.h"

#include <cassert>

#include "lib/exception.h"
#include "lib/string.h"
#include "lib/io/stream.h"

CsvParser::CsvParser(shared_ptr<InputStream> stream, char delim, char quote)
    : stream_(std::move(stream)), delim_(delim), quote_(quote), iLine_(0) {}

CsvParser::CsvParser(const string& filenameC) : CsvParser(openFileBufferedReader(filenameC)) { filename_ = filenameC; }

const string& CsvParser::filename() const { return filename_; }

bool CsvParser::readHeader() {
    if (!readLine()) {
        return false;
    }
    header_ = line_;
    fieldToIndex_.clear();
    for (size_t i = 0; i < header_.size(); ++i) {
        ASSERTEQ(fieldToIndex_.count(header_[i]), 0);
        fieldToIndex_.emplace(header_[i], i);
    }
    return true;
}

bool CsvParser::readLine() {
    if (!stream_->readLine(sLine_)) {
        return false;
    }
    if (sLine_.empty()) {
    	return false;
    }
    while (sLine_.size() && (sLine_.back() == '\r' || sLine_.back() == '\n')) {
    	sLine_.pop_back();
    }
    ++iLine_;
    line_.clear();
    bool inQuote = false;
    auto begin = sLine_.data();
    auto p = begin;
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
    if (p >= begin) {
        line_.emplace_back(begin, p - begin);
    }
    for (auto& s : line_) {
        unquote(s);
    }
    return true;
}

size_t CsvParser::size() const { return line_.size(); }

const StringVector& CsvParser::header() const { return header_; }

const string& CsvParser::get(size_t index) const {
    ASSERTLT(index, line_.size());
    return line_[index];
}

int CsvParser::getInt(size_t index) const { return stoi(get(index)); }

double CsvParser::getDouble(size_t index) const { return stod(get(index)); }

float CsvParser::getFloat(size_t index) const { return stof(get(index)); }

u64 CsvParser::getU64(size_t index) const { return stoll(get(index)); }

bool CsvParser::getBool(size_t index) const { return stringToBool(get(index)); }

bool CsvParser::empty(size_t index) const { return line_[index].empty(); }

int CsvParser::getIndex(const string& s) const { return findWithDefault(fieldToIndex_, s, -1); }

size_t CsvParser::getIndexOrDie(const string& column) const {
    auto result = getIndex(column);
    if (result == -1) {
        THROW("Column '" << column << "' not found");
    }
    return static_cast<size_t>(result);
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
                *w++ = quote_;
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

size_t CsvParser::lineIndex() const { return iLine_; }

std::string CsvParser::line() const { return sLine_; }

bool CsvParser::skipLines(size_t nLines) {
    for (size_t iLine = 0; iLine < nLines; ++iLine) {
        if (!stream_->readLine(sLine_)) {
            return false;
        }
    }
    return true;
}
