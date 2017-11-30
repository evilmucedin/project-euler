#pragma once

#include "lib/header.h"

class CsvParser {
public:
  CsvParser(shared_ptr<istream> stream, char delim = ',', char quote = '\"');
  bool readHeader();
  bool readLine();
  size_t size() const;
  const string& get(size_t index) const;
  int getIndex(const string& s) const;

private:
  void unquote(string& s);

  shared_ptr<istream> stream_;
  char delim_;
  char quote_;
  string sLine_;
  StringVector header_;
  unordered_map<string, int> fieldToIndex_;
  StringVector line_;
};
