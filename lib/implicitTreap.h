#pragma once

#include "glog/logging.h"

#include "lib/random.h"
#include "lib/header.h"

template<typename T>
class ImplicitTreap {
public:
    size_t size() const {
        return size(root_);
    }

    void push_back(const T& value) {
        auto sz = size();
        auto splitResult = split(root_, sz);
        auto newNode = allocate(value);
        root_ = merge(root_, newNode);
    }

    const T& at(size_t index) {
        auto splitResult = split(root_, index);
        Node* prev = nullptr;
        Node* now = splitResult.second;
        while (now) {
            prev = now;
            now = now->left_;
        }
        root_ = merge(splitResult.first, splitResult.second);
        return prev->value_;
    }

    void cutAndInsert(size_t begin, size_t end, size_t insertPoint) {
        auto split1 = split(root_, begin);
        auto split2 = split(split1.second, end - begin);
        if (insertPoint == 0) {
            root_ = merge(merge(split2.first, split1.first), split2.second);
        } else {
            root_ = merge(merge(split1.first, split2.second), split2.first);
        }
    }

private:
    struct Node {
        T value_;
        int priority_;
        Node* left_;
        Node* right_;
        size_t size_;
    };

    Node* allocate(const T& value) {
        auto result = new Node();
        result->value_ = value;
        result->priority_ = randInt();
        result->left_ = nullptr;
        result->right_ = nullptr;
        result->size_ = 1;
        return result;
    }

    std::pair<Node*, Node*> split(Node* node, int k) {
        if (!node) {
            return {nullptr, nullptr};
        }
        int l = size(node->left_);
        if (l >= k) {
            auto splitResult = split(node->left_, k);
            node->left_ = splitResult.second;
            updateSize(node);
            return {splitResult.first, node};
        } else {
            auto splitResult = split(node->right_, k - l - 1);
            node->right_ = splitResult.first;
            updateSize(node);
            return {node, splitResult.second};
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
            updateSize(t1);
            return t1;
        } else {
            t2->left_ = merge(t1, t2->left_);
            updateSize(t2);
            return t2;
        }
    }

    size_t size(const Node* node) const {
        if (!node) {
            return 0;
        }

        return node->size_;
    }

    void updateSize(Node* node) { node->size_ = 1 + size(node->left_) + size(node->right_); }

    Node* root_{nullptr};
};
