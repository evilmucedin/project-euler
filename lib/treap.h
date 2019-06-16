#pragma once

#include "lib/random.h"

template <typename K, typename V>
class Treap {
    struct Node {
        K key_;
        V value_;
        int priority_;
        Node* left_;
        Node* right_;
    };

    void emplace(const K& key, const V& value) {
        Node* t1;
        Node* t2;
        std::tie(t1, t2) = split(root_, key);
        auto newNode = allocate(key, value);

    }

   private:
    Node* allocate(const K& key, const V& value) {
        auto result = new Node();
        result->key_ = key;
        result->value = value;
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
            auto splitRight = split(n->right, key);
            n->right = splitRight->first;
            return {n, splitRight->second};
        } else {
            auto splitLeft = split(n->left, key);
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
        if (t1->priority_ > t2.priority_) {
            t1->right_ = merge(t1->right_, t2);
            return t1;
        } else {
            t2->left_ = merge(t1, t2->right_);
            return t2;
        }
    }

    Node* root_;
};
