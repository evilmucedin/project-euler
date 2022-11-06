#pragma once

#include <memory>
#include <vector>

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

        PTrieNode& getNode(T ch) { return subnodes_[ch - trie_->min_]; }

        const PTrieNode getNode(T ch) const { return subnodes_[ch - trie_->min_]; }
    };

    PTrieNode root_;
    T min_;
    T max_;
    const size_t n_;

    Trie(T min, T max) : min_(min), max_(max), n_(max_ - min_ + 1) {}

    PTrieNode createNode() {
        auto result = make_shared<TrieNode>();
        result->subnodes_.resize(n_);
        result->trie_ = this;
        result->word_ = false;
        return result;
    }

    template <typename C>
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
                THROW("unsupported trie character '" << ch << "'");
            }
        }
        curr->word_ = true;
        curr->extra_ = e;
    }

    template <typename C>
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
};

