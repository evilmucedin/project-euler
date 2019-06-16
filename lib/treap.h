#pragma once

#include "lib/random.h"

template <typename K, typename V>
class Treap {
    struct Node {
        K key_;
        V value_;
        int priority_;
    };

   private:
    Node* root_;
};
