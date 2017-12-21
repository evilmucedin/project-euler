#pragma once

#include "lib/header.h"

class CsvParser {
   public:
    CsvParser(shared_ptr<istream> stream, char delim = ',', char quote = '\"');
    bool readHeader();
    bool readLine();
    size_t size() const;

    const string& get(size_t index) const;
    int getInt(size_t index) const;
    double getDouble(size_t index) const;
    bool empty(size_t index) const;

    int getIndex(const string& s) const;
    size_t line() const;
    bool skipLines(size_t nLines);

   private:
    void unquote(string& s);

    shared_ptr<istream> stream_;
    char delim_;
    char quote_;
    string sLine_;
    StringVector header_;
    unordered_map<string, int> fieldToIndex_;
    StringVector line_;
    size_t iLine_;
};
