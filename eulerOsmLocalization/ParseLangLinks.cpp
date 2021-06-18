#include <cstdio>

#include "glog/logging.h"

#include "lib/header.h"
#include "lib/string.h"

static constexpr char FILENAME[] = "eulerOsmLocalization/enwiki-20210601-langlinks.sql";

int main() {
    setlocale(LC_ALL, "");

    FILE* fIn = fopen(FILENAME, "rb");
    WChar ch;
    size_t count = 0;
    size_t tuples = 0;
    size_t state = 0;
    WString token;

    struct LangPair {
        WString lang;
        WString title;
    };

    unordered_map<WString, vector<LangPair>> langLinks;
    while (WEOF != (ch = fgetwc(fIn))) {
        switch (state) {
            case 0:
                if (ch == '(') {
                    state = 1;
                    token.clear();
                }
                break;
            case 1:
                if (ch == ')') {
                    const auto parts = split(token, ',');
                    if (parts.size() == 3) {
                        langLinks[parts[0]].emplace_back(LangPair{unquote(parts[1]), unquote(parts[2])});
                        ++tuples;
                    }
                    state = 2;
                } else {
                    token += ch;
                }
                break;
            case 2:
                if (ch == '(') {
                    state = 0;
                }
                break;
        }

        ++count;
        if (0 == count % 10000) {
            LOG_EVERY_MS(INFO, 1000) << OUT(bytesToStr(count)) << OUT(tuples) << OUT(langLinks.size());
        }
    }

    LOG(INFO) << OUT(count) << OUT(tuples) << OUT(langLinks.size());

    fclose(fIn);

    FILE* fOut = fopen("eulerOsmLocalization/enwiki-langlings.tsv", "wb");
    for (const auto& langlink : langLinks) {
        FPutWString(fOut, langlink.first);
        fprintf(fOut, "\t%zd", langlink.second.size());
        for (const auto& p : langlink.second) {
            fprintf(fOut, "\t");
            FPutWString(fOut, p.lang);
            fprintf(fOut, ",");
            FPutWString(fOut, p.title);
        }
        fprintf(fOut, "\n");
    }
    fclose(fOut);

    return 0;
}
