#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

struct Token {
    bool isValue_{};
    string arg1_;
    string op_;
    string arg2_;
    i64 value_;
};

ostream& operator<<(ostream& s, const Token& t) {
    s << "{" << t.isValue_ << ", " << t.arg1_ << ", " << t.value_ << "}";
    return s;
}

void first() {
    const auto input = readInputLines();

    unordered_map<string, Token> tokens;
    for (const string& s: input) {
        const auto parts = split(s, ' ');
        // cerr << parts << endl;
        string token = parts[0];
        token = replaceAll(token, ":", "");
        auto& t = tokens[token];
        if (parts.size() == 2) {
            t.isValue_ = true;
            t.value_ = atoi(parts[1].c_str());
        } else {
            t.isValue_ = false;
            t.arg1_ = parts[1];
            t.op_ = parts[2];
            t.arg2_ = parts[3];
        }
    }

    // cerr << map2Tuples(tokens) << endl;

    bool cont = true;
    while (cont) {
        cont = false;
        for (auto& tt: tokens) {
            auto& t = tt.second;
            if (!t.isValue_) {
                if (tokens[t.arg1_].isValue_ && tokens[t.arg2_].isValue_) {
                    cont = true;
                    i64 val1 = tokens[t.arg1_].value_;
                    i64 val2 = tokens[t.arg2_].value_;
                    t.isValue_ = true;
                    if (t.op_ == "+") {
                        t.value_ = val1 + val2;
                    } else if (t.op_ == "-") {
                        t.value_ = val1 - val2;
                    } else if (t.op_ == "*") {
                        t.value_ = val1 * val2;
                    } else if (t.op_ == "/") {
                        t.value_ = val1 / val2;
                    } else {
                        THROW("Unknown op")
                    }
                }
            }
        }
    }

    auto result = tokens["root"].value_;
    cout << result << endl;
}

using Tokens = unordered_map<string, Token>;

string expr(const string& tt, const Tokens& tokens) {
    if (tt == "") {
        return "";
    }
    if (tt == "x") {
        return "x";
    }
    const auto toT = tokens.find(tt);
    const auto& t = toT->second;
    if (t.isValue_) {
        return to_string(t.value_);
    } else {
        return "(" + expr(t.arg1_, tokens) + " " + t.op_ + " " + expr(t.arg2_, tokens) + ")";
    }
}

i64 eval(i64 x, const string& tt, const Tokens& tokens) {
    if (tt == "") {
        return 0;
    }
    if (tt == "x") {
        return x;
    }
    const auto toT = tokens.find(tt);
    const auto& t = toT->second;
    if (t.isValue_) {
        return t.value_;
    } else {
        auto v1 = eval(x, t.arg1_, tokens);
        auto v2 = eval(x, t.arg2_, tokens);
        if (t.op_ == "+") {
            return v1 + v2;
        } else if (t.op_ == "/") {
            return v1 / v2;
        } else if (t.op_ == "*") {
            return v1 * v2;
        } else if (t.op_ == "-") {
            return v1 - v2;
        }
    }
}

void second() {
    const auto input = readInputLines();

    Tokens tokens;
    for (const string& s: input) {
        const auto parts = split(s, ' ');
        // cerr << parts << endl;
        string token = parts[0];
        token = replaceAll(token, ":", "");

        auto& t = tokens[token];
        if (token == "root") {
            t.isValue_ = false;
            t.arg1_ = parts[1];
            t.op_ = "==";
            t.arg2_ = parts[3];
        } else if (token == "humn") {
            t.isValue_ = false;
            t.arg1_ = "x";
            t.op_ = "";
            t.arg2_ = "";
        } else if (parts.size() == 2) {
            t.isValue_ = true;
            t.value_ = atoi(parts[1].c_str());
        } else {
            t.isValue_ = false;
            t.arg1_ = parts[1];
            t.op_ = parts[2];
            t.arg2_ = parts[3];
        }
    }

    bool cont = true;
    while (cont) {
        cont = false;
        for (auto& tt: tokens) {
            auto& t = tt.second;
            if (!t.isValue_) {
                if (tokens[t.arg1_].isValue_ && tokens[t.arg2_].isValue_) {
                    cont = true;
                    i64 val1 = tokens[t.arg1_].value_;
                    i64 val2 = tokens[t.arg2_].value_;
                    t.isValue_ = true;
                    if (t.op_ == "+") {
                        t.value_ = val1 + val2;
                    } else if (t.op_ == "-") {
                        t.value_ = val1 - val2;
                    } else if (t.op_ == "*") {
                        t.value_ = val1 * val2;
                    } else if (t.op_ == "/") {
                        t.value_ = val1 / val2;
                    } else {
                        THROW("Unknown op")
                    }
                }
            }
        }
    }

    cout << expr("root", tokens) << endl;

    i64 l = -4016936880448;
    i64 r = 4016936880448;

    for (int i = 0; i < 100; ++i) {
        i64 m = (r + l) / 2;
        i64 fm = eval(m, "fgtg", tokens);
        cerr << m << " " << fm << endl;
        if (fm > 13751780524553ULL) {
            l = m;
        } else {
            r = m;
        }
    }

    cerr << l << " " << r << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}

