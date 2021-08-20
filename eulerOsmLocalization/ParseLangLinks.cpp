#include <cstdio>

#include "glog/logging.h"

#include "lib/header.h"
#include "lib/string.h"
#include "lib/file.h"

struct TupleParser {
    TupleParser(File& file) : file_(file), bufFile_(file_), tuples_(0) {}

    struct Tuple {
        WStringVector tokens_;
    };

    bool nextTuple(Tuple& tuple) {
        tuple.tokens_.clear();

        while (!bufFile_.eof() && bufFile_.peek() != '(') {
            bufFile_.advance();
        }
        if (bufFile_.peek() == '(') {
            bufFile_.advance();
        }

        while (!bufFile_.eof() && (bufFile_.peek() != ')') && nextToken(tuple)) {
            // LOG_EVERY_MS(INFO, 1000) << OUT(tuple.tokens_.size()) << OUT(static_cast<char>(bufFile_.peek()));
            if (bufFile_.peek() == ',') {
                bufFile_.advance();
            }
        }

        ++tuples_;

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
            // LOG_EVERY_MS(INFO, 1000) << OUT(bytesToStr(bufFile_.offset())) << OUT(static_cast<char>(bufFile_.peek()))
            //                          << OUT(token.size()) << OUT(tuples_);
        }
        if (bufFile_.peek() == '\'') {
            bufFile_.advance();
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

    size_t tuples() const {
        return tuples_;
    }

    File& file_;
    BufferedFileReader bufFile_;
    size_t tuples_;
};

template<typename T>
void parseTuples(File& fIn, T callback) {
    TupleParser parser(fIn);
    TupleParser::Tuple tuple;
    while (parser.nextTuple(tuple)) {
        callback(tuple.tokens_);

        if (0 == parser.tuples() % 1000) {
            LOG_EVERY_MS(INFO, 1000) << OUT(bytesToStr(parser.offset())) << OUT(parser.tuples())
                                     << OUT(tuple.tokens_.size()) << OUT(parser.tuples());
        }
    }

    LOG(INFO) << OUT(parser.offset()) << OUT(parser.tuples());
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
        // LOG_EVERY_MS(INFO, 1000) << OUT(parts.size());
        if (parts.size() == 13) {
            // fwprintf(stderr, L"%ls\n", parts[0].data(), UTF8_NEW_LINE);
            // fPutWString(stderr, parts[0]);
            // fprintf(stderr, "%s\n", UTF8_NEW_LINE);
            try {
                // fwprintf(stderr, L"%ls %ls %ls\n", parts[0].data(), parts[1].data(), parts[2].data());
                pageTitles[wStringToU64(parts[0])] = parts[2];
            } catch (...) {
                fwprintf(stderr, L"Bad token '%ls' %zd\n", parts[0].data(), fIn.offset());
            }
        } else {
            fwprintf(stderr, L"Bad token '%ls' %zd %s\n", parts[0].data(), parts.size(),
                     bytesToStr(fIn.offset()).c_str());
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

template<typename Callback>
void parseArticles(File& fIn, const WString& open, const WString& close, Callback cb) {
    WString vctOpen;
    WString vctContent;
    WString vctClose;
    while (!fIn.eof()) {
        vctOpen.clear();
        while (!fIn.eof() && (vctOpen.size() != open.size())) {
            auto next = fIn.getUTF8C();
            if (next == open[vctOpen.size()]) {
                vctOpen.emplace_back(next);
            } else {
                vctOpen.clear();
                if (next == open[vctOpen.size()]) {
                    vctOpen.emplace_back(next);
                }
            }
        }

        vctContent.clear();
        vctClose.clear();
        while (!fIn.eof() && (vctClose.size() != close.size())) {
            auto next = fIn.getUTF8C();
            if (next == close[vctClose.size()]) {
                vctClose.emplace_back(next);
            } else {
                vctContent.insert(vctContent.end(), vctClose.begin(), vctClose.end());
                vctClose.clear();
                if (next == close[vctClose.size()]) {
                    vctClose.emplace_back(next);
                } else {
                    vctContent.emplace_back(next);
                }
            }
        }

        if (vctContent.size()) {
            vctContent.emplace_back(0);
            cb(vctContent);
        }
    }
}

void parseEnArticles() {
    static constexpr char FILENAME[] = "eulerOsmLocalization/enwiki-20210601-pages-articles-multistream.xml";

    File fIn(FILENAME, "rb");
    size_t count = 0;
    parseArticles(fIn, stringToWString("<page>"), stringToWString("</page>"), [&](const WString& article) {
        // fwprintf(stderr, L"%ls\n", article.data());
        // fwprintf(stderr, L"-----------------------------\n");
        ++count;
        LOG_EVERY_MS(INFO, 10000) << count << " " << bytesToStr(fIn.offset());
    });

    LOG(INFO) << "Found " << count << " articles";
}

int main() {
    setlocale(LC_ALL, "en_US.UTF8");

    // parseEnTitles();
    // parseTitles();
    // parseLangLinks();
    parseEnArticles();

    return 0;
}
