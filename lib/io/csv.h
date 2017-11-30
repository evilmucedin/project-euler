#pragma once

#include "lib/header.h"

class CsvParser {
public:
  CsvParser(shared_ptr<istream> stream, char delim = ',', char quote = '\"');
  bool readHeader();
  bool readLine();
  size_t size() const;
  const string& get(size_t index) const;

private:
  shared_ptr<istream> stream_;
  char delim_;
  char quote_;
  string sLine_;
  StringVector header_;
  StringVector line_;
};
