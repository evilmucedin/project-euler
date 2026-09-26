#pragma once

#include <iostream>

#include "lib/header.h"
#include "lib/io/stream.h"

template<typename T>
void saveVector(ostream& s, const vector<T>& vct) {
    s << vct.size();
    for (const auto& x: vct) {
        s << "\t" << x;
    }
}

template<typename T>
void saveVector(OutputStream& s, const vector<T>& vct) {
    s << vct.size();
    for (const auto& x: vct) {
        s << "\t" << x;
    }
}

template<typename T>
vector<T> loadVector(istream& s) {
    size_t size;
    s >> size;
    vector<T> result(size);
    for (size_t i = 0; i < size; ++i) {
        s >> result[i];
    }
    return result;
}

template<typename T>
vector<T> loadVector(InputStream& s) {
    size_t size;
    s >> size;
    vector<T> result(size);
    for (size_t i = 0; i < size; ++i) {
        s >> result[i];
    }
    return result;
}

vector<string> loadAsLines(const string& filename);

string cwd();
string joinPath(const StringVector& parts);
string simplifyPath(const string& path);
string getAbsolutePath(const string& filename);
string getDir(const string& filename);

string repoRoot();

bool isFile(const string& filename);
