#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

struct Node;

using PNode = shared_ptr<Node>;

struct Node {
    string name_;
    i64 size_{};
    bool dir_{};

    unordered_map<string, PNode> nodes_;

    PNode createSubnode(string name, bool dir, i64 size = 0) {
        if (nodes_.count(name)) {
            return nodes_[name];
        }
        auto n = make_shared<Node>();
        n->name_ = name;
        n->dir_ = dir;
        n->size_ = size;
        nodes_.emplace(name, n);
        return n;
    }

    i64 totalSize() const {
        if (dir_) {
            i64 result = 0;
            for (const auto& p: nodes_) {
                result += p.second->totalSize();
            }
            return result;
        } else {
            return size_;
        }
    }

    i64 answer() const {
        if (dir_) {
            i64 result = 0;
            auto ts = totalSize();
            if (ts <= 100000) {
                result += ts;
            }
            for (const auto& p: nodes_) {
                result += p.second->answer();
            }
            return result;
        }
        return 0;
    }

    i64 answer2(i64 threshold) const {
        if (dir_) {
            i64 result = 70000000;
            auto ts = totalSize();
            if (ts >= threshold) {
                result = std::min(result, ts);
            }
            for (const auto& p: nodes_) {
                result = std::min(result, p.second->answer2(threshold));
            }
            return result;
        }
        return 70000000;
    }

    void print(int level) const {
        cerr << string(level, ' ') << name_ << " " << dir_ << " " << size_ << endl;
        for (const auto& p: nodes_) {
            p.second->print(level + 1);
        }
    }
};

void first() {
    const auto input = readInputLines();

    auto root = make_shared<Node>();
    root->dir_ = true;

    auto now = root;
    vector<PNode> s;
    for (int i = 0; i < input.size(); ++i) {
        // cerr << input[i] << endl;
        if (input[i][0] == '$') {
            const auto tokens = split(input[i], ' ');
            if (tokens[1] == "cd") {
                if (tokens[2] == "..") {
                    if (s.empty()) {
                        THROW("Empty dir stack");
                    }
                    now = s.back();
                    s.pop_back();
                } else {
                    if (tokens[2] != "/") {
                        s.emplace_back(now);
                        now = now->createSubnode(tokens[2], true);
                    } else {
                        s.clear();
                        now = root;
                    }
                }
            } else if (tokens[1] == "ls") {
                while (i + 1 < input.size() && input[i + 1][0] != '$') {
                    const auto tokens = split(input[i + 1], ' ');
                    if (tokens[0] == "dir") {
                        now->createSubnode(tokens[1], true);
                    } else {
                        // cerr << "Create " << tokens[1] << " with size " << tokens[0] << endl;
                        now->createSubnode(tokens[1], false, atol(tokens[0].c_str()));
                    }
                    ++i;
                }
            } else {
                THROW("Unknown command: " << tokens[1]);
            }
        } else {
            THROW("Bad input");
        }
    }

    // cout << "TS: " << root->totalSize() << endl;
    // root->print(0);
    cout << root->answer() << endl;

    const auto free = 70000000 - root->totalSize();
    const auto need = 30000000 - free;
    cerr << "need: " << need << endl;
    cout << root->answer2(need) << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
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

