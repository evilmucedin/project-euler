#pragma once

#include "lib/header.h"

class InputStream;

class CsvParser {
   public:
    CsvParser(shared_ptr<InputStream> stream, char delim = ',', char quote = '\"');
    CsvParser(const string& filename);
    const string& filename() const;

    bool readHeader();
    bool readLine();
    size_t size() const;
    const StringVector& header() const;

    const string& get(size_t index) const;
    int getInt(size_t index) const;
    double getDouble(size_t index) const;
    float getFloat(size_t index) const;
    u64 getU64(size_t index) const;
    bool getBool(size_t index) const;
    bool empty(size_t index) const;

    int getIndex(const string& s) const;
    size_t getIndexOrDie(const string& column) const;
    size_t lineIndex() const;
    bool skipLines(size_t nLines);
    std::string line() const;

   private:
    void unquote(string& s);

    string filename_;
    shared_ptr<InputStream> stream_;
    char delim_;
    char quote_;
    string sLine_;
    StringVector header_;
    unordered_map<string, int> fieldToIndex_;
    StringVector line_;
    size_t iLine_;
};
