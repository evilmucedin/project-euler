#include "utils.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>

#include "glog/logging.h"
#include "lib/string.h"

vector<string> loadAsLines(const string& filename) {
    vector<string> result;
    ifstream fIn(filename);
    string s;
    while (getline(fIn, s)) {
        result.emplace_back(std::move(s));
    }
    return result;
}

string cwd() {
    char szTmp[32];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    static constexpr size_t kLen = 1024;
    char pBuf[kLen];
    size_t len = kLen;
    int bytes = min<int>(readlink(szTmp, pBuf, len), len - 1);
    if (bytes >= 0) {
        pBuf[bytes] = '\0';
    }
    return pBuf;
}

string joinPath(const StringVector& parts) {
    StringVector result;
    for (const auto& p: parts) {
        if (p == "..") {
            if (!result.empty()) {
                result.pop_back();
            } else {
                result.emplace_back(p);
            }
        } else if (p == ".") {
        } else {
            result.emplace_back(p);
        }
    }
    return join("/", result);
}

string simplifyPath(const string& path) {
    return joinPath(split(path, '/'));
}

string getAbsolutePath(const string& filename) {
    char absolutePath[1 << 13];
    return realpath(filename.c_str(), absolutePath);
}

string getDir(const string& filename) {
    const auto parts = split(filename, '/');
    return joinPath(StringVector(parts.begin(), parts.end() - 1));
}

string repoRoot() {
    const auto parts = split(cwd(), '/');
    static const string kRoot = "project-euler";
    size_t i = 0;
    while (i < parts.size() && parts[i] != kRoot) {
        ++i;
    }
    if (i == parts.size()) {
        LOG(FATAL) << kRoot << " not found in the current path";
    }
    return joinPath(StringVector(parts.begin(), parts.begin() + i + 1));
}

bool isFile(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}
