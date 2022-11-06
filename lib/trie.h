#pragma once

template <typename T, typename Extra = void>
struct Trie {
    struct TrieNode;
    using PTrieNode = shared_ptr<TrieNode>;

    struct TrieNode {
        vector<PTrieNode> subnodes_;
        Extra extra_;
    };

    PTrieNode root_;
    const size_t n_;

    Trie(T min, T max) : n_(max - min + 1) {}

    template<typename C>
    void addWord(const C& s) {

    }
};
