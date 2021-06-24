#include <cstdio>

#include "glog/logging.h"

#include "lib/header.h"
#include "lib/string.h"
#include "lib/file.h"

static constexpr char FILENAME[] = "eulerOsmLocalization/enwiki-20210601-langlinks.sql";
static constexpr char UTF8_NEW_LINE[] = "\u2424";
// static constexpr char UTF8_NEW_LINE[] = {(char)0xE2, (char)0x90, (char)0xA4};

void parseLangLinks() {
    // File fIn(FILENAME, "rb,ccs=UTF-8");
    File fIn(FILENAME, "rb");
    size_t count = 0;
    size_t tuples = 0;
    size_t state = 0;
    WString token;
    WString prevToken;

    struct LangPair {
        WString lang;
        WString title;
    };

    unordered_map<uint64_t, vector<LangPair>> langLinks;
    while (!fIn.eof()) {
        WChar ch = fIn.getUTF8C();
        /*
        if (count < 10000) {
            fwprintf(stderr, L"%lc", ch);
        }
        */
        if (ch != WEOF) {
            switch (state) {
                case 0:
                    if (ch == '(') {
                        state = 1;
                        prevToken.swap(token);
                        token.clear();
                    }
                    break;
                case 1:
                    if (ch == ')') {
                        const auto parts = split(token, ',');
                        if (parts.size() == 3) {
                            // fwprintf(stderr, L"%ls\n", parts[0].data(), UTF8_NEW_LINE);
                            // fPutWString(stderr, parts[0]);
                            // fprintf(stderr, "%s\n", UTF8_NEW_LINE);
                            try {
                                // fwprintf(stderr, L"%ls %ls %ls\n", parts[0].data(), parts[1].data(),
                                // parts[2].data());
                                langLinks[wStringToU64(parts[0])].emplace_back(
                                    LangPair{unquote(parts[1], '\''), unquote(parts[2], '\'')});
                            } catch (...) {
                                token.emplace_back(0);
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
        }

        ++count;
        if (0 == count % 10000) {
            LOG_EVERY_MS(INFO, 1000) << OUT(bytesToStr(count)) << OUT(tuples) << OUT(langLinks.size());
        }
    }

    LOG(INFO) << OUT(count) << OUT(tuples) << OUT(langLinks.size());
    prevToken.emplace_back(0);
    fwprintf(stdout, L"prev token: '%ls'\n", prevToken.data());

    fIn.close();

    FILE* fOut = fopen("eulerOsmLocalization/enwiki-langlings.tsv", "wb,ccs=UTF-8");
    for (const auto& langlink : langLinks) {
        if (langlink.second.size() < 3) {
            continue;
        }
        fwprintf(fOut, L"%zd\t%zd", langlink.first, langlink.second.size());
        for (const auto& p : langlink.second) {
            fwprintf(fOut, L"\t%ls,%ls", p.lang.data(), p.title.data());
        }
        fwprintf(fOut, L"\n");
    }
    fclose(fOut);
}

int main() {
    setlocale(LC_ALL, "en_US.UTF8");

    ParseLangLinks();

    return 0;
}
