#include "lib/io/utils.h"

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

string repoRoot() {
    auto parts = split(cwd(), '/');
    static const string kRoot = "project-euler";
    size_t i = 0;
    while (i < parts.size() && parts[i] != kRoot) {
        ++i;
    }
    if (i == parts.size()) {
        LOG(FATAL) << kRoot << " not found in the current path";
    }
    return join("/", StringVector(parts.begin(), parts.begin() + i + 1));
}

