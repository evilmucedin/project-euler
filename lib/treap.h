#pragma once

#include "lib/header.h"

#include "lib/random.h"

template <typename K, typename V>
class Treap {
public:
    void emplace(const K& key, const V& value) {
        auto splitResult = split(root_, key);
        auto newNode = allocate(key, value);
        auto m1 = merge(splitResult.first, newNode);
        root_ = merge(m1, splitResult.second);
    }

    size_t size() const {
        return size(root_);
    }

    void erase(const K& key) {
        auto splitResult = split(root_, key);
        Node* now = splitResult.second;
        Node* prev = nullptr;
        Node* prevprev = nullptr;
        while (now) {
            prevprev = prev;
            prev = now;
            now = now->left_;
        }
        assert(prev->key_ == key);
        if (prevprev) {
            prevprev->left_ = nullptr;
        } else {
            splitResult.second = nullptr;
        }
        delete prev;
        root_ = merge(splitResult.first, splitResult.second);
    }

    V get(const K& key) {
        auto now = root_;
        while (now->key_ != key) {
            if (now->key_ < key) {
                now = now->left_;
            } else {
                now = now->right_;
            }
        }
        return now->value_;
    }

   private:
    struct Node {
        K key_;
        V value_;
        int priority_;
        Node* left_;
        Node* right_;
    };

    Node* allocate(const K& key, const V& value) {
        auto result = new Node();
        result->key_ = key;
        result->value_ = value;
        result->priority_ = randInt();
        result->left_ = nullptr;
        result->right_ = nullptr;
        return result;
    }

    std::pair<Node*, Node*> split(Node* n, const K& key) {
        if (n == nullptr) {
            return {nullptr, nullptr};
        }
        if (n->key_ > key) {
            auto splitRight = split(n->right_, key);
            n->right_ = splitRight.first;
            return {n, splitRight.second};
        } else {
            auto splitLeft = split(n->left_, key);
            n->left_ = splitLeft.second;
            return {splitLeft.first, n};
        }
    }

    Node* merge(Node* t1, Node* t2) {
        if (!t2) {
            return t1;
        }
        if (!t1) {
            return t2;
        }
        if (t1->priority_ > t2->priority_) {
            t1->right_ = merge(t1->right_, t2);
            return t1;
        } else {
            t2->left_ = merge(t1, t2->left_);
            return t2;
        }
    }

    size_t size(const Node* n) const {
        if (!n) {
            return 0;
        }
        return 1 + size(n->left_) + size(n->right_);
    }

    Node* root_{};
};
