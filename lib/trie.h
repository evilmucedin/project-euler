#pragma once

#include <memory>
#include <vector>
#include <string>

#include <lib/exception.h>

using namespace std;

struct EmptyExtra {
};

template <typename T, typename Extra = EmptyExtra>
struct Trie {
    struct TrieNode;
    using PTrieNode = shared_ptr<TrieNode>;

    struct TrieNode {
        vector<PTrieNode> subnodes_;
        bool word_;
        Extra extra_;
        Trie* trie_;

        bool isWord() const {
            return word_;
        }

        PTrieNode& getNode(T ch) {
            if (ch < trie_->min_ || ch > trie_->max_) {
                THROW("bad character");
            }
            return subnodes_[ch - trie_->min_];
        }

        const PTrieNode& getNodeConst(T ch) const {
            if (ch < trie_->min_ || ch > trie_->max_) {
                THROW("bad character");
            }
            return subnodes_[ch - trie_->min_];
        }

        void print(int indent = 0) const {
            string s;
            for (int i = 0; i < indent; ++i) {
                s += " ";
            }
            s += "(";
            for (T ch = trie_->min_; ch <= trie_->max_; ++ch) {
                if (subnodes_[ch - trie_->min_]) {
                    s += ch;
                }
            }
            s += ") w=";
            s += (word_) ? "+" : "-";
            cerr << s << endl;
            for (const auto& p: subnodes_) {
                if (p) {
                    p->print(indent + 1);
                }
            }
        }
    };

    PTrieNode root_;
    T min_;
    T max_;
    const size_t n_;

    Trie(T min, T max) : min_(min), max_(max), n_(max_ - min_ + 1) {}

    PTrieNode root() {
        return root_;
    }

    PTrieNode createNode() {
        auto result = make_shared<TrieNode>();
        result->subnodes_.resize(n_);
        result->trie_ = this;
        result->word_ = false;
        return result;
    }

    template <typename C = string>
    void addWord(const C& s, Extra e = Extra()) {
        if (root_ == nullptr) {
            root_ = createNode();
        }
        PTrieNode curr = root_;
        for (auto ch : s) {
            if (ch >= min_ && ch <= max_) {
                PTrieNode& pNode = curr->getNode(ch);
                if (!pNode) {
                    pNode = createNode();
                }
                curr = pNode;
            } else {
                THROW("unsupported trie character '" << static_cast<int>(ch) << "'");
            }
        }
        curr->word_ = true;
        curr->extra_ = e;
    }

    template <typename C = string>
    PTrieNode findNode(const C& s) {
        PTrieNode curr = root_;
        for (auto ch : s) {
            if (!curr) {
                return nullptr;
            }
            curr = curr->getNode(ch);
            if (!curr) {
                return nullptr;
            }
        }
        return curr;
    }

    template<typename C>
    bool hasWord(const C& s) {
        const auto nd = findNode(s);
        if (!nd) {
            return false;
        }
        return nd->word_;
    }

    void print() const {
        if (root_) {
            root_->print();
        }
    }
};

