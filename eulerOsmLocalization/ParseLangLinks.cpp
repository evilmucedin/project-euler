#include <cstdio>

#include "glog/logging.h"

#include "lib/header.h"
#include "lib/string.h"

static constexpr char FILENAME[] = "eulerOsmLocalization/enwiki-20210601-langlinks.sql";
static constexpr char UTF8_NEW_LINE[] = "\u2424";
// static constexpr char UTF8_NEW_LINE[] = {(char)0xE2, (char)0x90, (char)0xA4};

int main() {
    setlocale(LC_ALL, "en_US.UTF8");

    FILE* fIn = fopen(FILENAME, "rb,ccs=UTF-8");
    WChar ch;
    size_t count = 0;
    size_t tuples = 0;
    size_t state = 0;
    WString token;

    struct LangPair {
        WString lang;
        WString title;
    };

    unordered_map<uint64_t, vector<LangPair>> langLinks;
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
                    token.emplace_back(0);
                    const auto parts = split(token, ',');
                    if (parts.size() == 3) {
                        // fwprintf(stderr, L"%ls\n", parts[0].data(), UTF8_NEW_LINE);
                        // fPutWString(stderr, parts[0]);
                        // fprintf(stderr, "%s\n", UTF8_NEW_LINE);
                        try {
                        langLinks[wStringToU64(parts[0])].emplace_back(
                            LangPair{unquote(parts[1], '\''), unquote(parts[2], '\'')});
                        } catch (...) {
                            fwprintf(stderr, L"Bad token '%ls'\n", token.data());
                        }
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

    FILE* fOut = fopen("eulerOsmLocalization/enwiki-langlings.tsv", "wb,ccs=UTF-8");
    for (const auto& langlink : langLinks) {
        fwprintf(fOut, L"%zd\t%zd", langlink.first, langlink.second.size());
        for (const auto& p : langlink.second) {
            fwprintf(fOut, L"\t%ls,%ls", p.lang.data(), p.title.data());
        }
        fprintf(fOut, "\n");
        fflush(fOut);
    }
    fclose(fOut);

    return 0;
}
