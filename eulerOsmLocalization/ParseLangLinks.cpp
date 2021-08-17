#include <cstdio>

#include "glog/logging.h"

#include "lib/header.h"
#include "lib/string.h"
#include "lib/file.h"

struct TupleParser {
    TupleParser(File& file) : file_(file), bufFile_(file_) {}

    struct Tuple {
        WStringVector tokens_;
    };

    bool nextTuple(Tuple& tuple) {
        tuple.tokens_.clear();

        auto ch = bufFile_.advance();
        while (ch != WEOF && ch != '(') {
            ch = bufFile_.advance();
        }

        while ((bufFile_.peek() != ')') && nextToken(tuple)) {
            if (bufFile_.peek() == ',') {
                bufFile_.advance();
            }
        }

        return !tuple.tokens_.empty();
    }

    bool nextToken(Tuple& tuple) {
        if (bufFile_.peek() == '\'') {
            return nextQuotedString(tuple);
        } else {
            return nextString(tuple);
        }
    }

    bool nextQuotedString(Tuple& tuple) {
        ASSERTEQ(bufFile_.peek(), '\'');
        bufFile_.advance();
        WString token;
        while (!bufFile_.eof() && (bufFile_.peek() != '\'')) {
            auto next = bufFile_.advance();
            if (next != '\\') {
                token.emplace_back(next);
            } else {
                token.emplace_back(bufFile_.advance());
            }
            LOG_EVERY_MS(INFO, 1000) << OUT(bytesToStr(bufFile_.offset())) << OUT(static_cast<char>(bufFile_.peek()))
                                     << OUT(token.size());
        }
        token.emplace_back(0);
        tuple.tokens_.emplace_back(std::move(token));
        return true;
    }

    bool nextString(Tuple& tuple) {
        ASSERTNEQ(bufFile_.peek(), '\'');
        WString token;
        while (!bufFile_.eof() && bufFile_.peek() != ',' && bufFile_.peek() != ')') {
            token.emplace_back(bufFile_.advance());
        }
        const auto len = token.size();
        token.emplace_back(0);
        tuple.tokens_.emplace_back(std::move(token));
        return len != 0;
    }

    u64 offset() const {
        return bufFile_.offset();
    }

    File& file_;
    BufferedFileReader bufFile_;
};

template<typename T>
void parseTuples(File& fIn, T callback) {
    size_t count = 0;
    size_t tuples = 0;

    TupleParser parser(fIn);
    TupleParser::Tuple tuple;
    while (parser.nextTuple(tuple)) {
        callback(tuple.tokens_);
        count = parser.offset();
        ++tuples;

        if (0 == tuples % 1000) {
            LOG_EVERY_MS(INFO, 1000) << OUT(bytesToStr(count)) << OUT(tuples) << OUT(tuple.tokens_.size());
        }
    }

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
    parseTuples(fIn, [&](const WStringVector& parts) {
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
                fwprintf(stderr, L"Bad token '%ls'\n", parts[0].data());
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

void parseEnTitles() {
    static constexpr char FILENAME[] = "eulerOsmLocalization/enwiki-20210620-page.sql";

    File fIn(FILENAME, "rb");
    unordered_map<uint64_t, WString> pageTitles;
    parseTuples(fIn, [&](const WStringVector& parts) {
        if (parts.size() == 13) {
            // fwprintf(stderr, L"%ls\n", parts[0].data(), UTF8_NEW_LINE);
            // fPutWString(stderr, parts[0]);
            // fprintf(stderr, "%s\n", UTF8_NEW_LINE);
            try {
                // fwprintf(stderr, L"%ls %ls %ls\n", parts[0].data(), parts[1].data(),
                // parts[2].data());
                pageTitles[wStringToU64(parts[0])] = unquote(parts[2]);
            } catch (...) {
                fwprintf(stderr, L"Bad token '%ls'\n", parts[0].data());
            }
        } else {
            fwprintf(stderr, L"Bad token '%ls' %zd\n", parts[0].data(), parts.size());
        }
    });
    LOG(INFO) << OUT(pageTitles.size());

    fIn.close();

    FILE* fOut = fopen("eulerOsmLocalization/enwiki-pagetitles.tsv", "wb,ccs=UTF-8");
    for (const auto& pageTitle : pageTitles) {
        fwprintf(fOut, L"%zd\t%ls\n", pageTitle.first, pageTitle.second.data());
    }
    fclose(fOut);
}

int main() {
    setlocale(LC_ALL, "en_US.UTF8");

    parseEnTitles();
    // parseTitles();
    // parseLangLinks();

    return 0;
}
