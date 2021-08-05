#include <cstdio>

#include "glog/logging.h"

#include "lib/header.h"
#include "lib/string.h"
#include "lib/file.h"

template<typename T>
void parseTuples(File& fIn, T callback) {
    size_t count = 0;
    size_t tuples = 0;
    size_t state = 0;
    WString token;
    WString prevToken;

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
                        callback(token);
                        state = 2;
                        ++tuples;
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
            LOG_EVERY_MS(INFO, 1000) << OUT(bytesToStr(count)) << OUT(tuples);
        }
    }

    prevToken.emplace_back(0);
    fwprintf(stdout, L"prev token: '%ls'\n", prevToken.data());
    LOG(INFO) << OUT(count) << OUT(tuples);
}

void parseLangLinks() {
    static constexpr char FILENAME[] = "eulerOsmLocalization/enwiki-20210601-langlinks.sql";

    // File fIn(FILENAME, "rb,ccs=UTF-8");
    File fIn(FILENAME, "rb");

    struct LangPair {
        WString lang;
        WString title;
    };

    unordered_map<uint64_t, vector<LangPair>> langLinks;
    parseTuples(fIn, [&](WString& token) {
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
        }
    });
    LOG(INFO) << OUT(langLinks.size());

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

void parseTitles() {
    static constexpr char FILENAME[] = "eulerOsmLocalization/enwiki-20210620-page.sql";

    // File fIn(FILENAME, "rb,ccs=UTF-8");
    File fIn(FILENAME, "rb");

}

int main() {
    setlocale(LC_ALL, "en_US.UTF8");

    parseTitles();

    // parseLangLinks();

    return 0;
}
