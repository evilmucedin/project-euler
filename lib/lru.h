#pragma once

#include <list>
#include <unordered_map>

#include "lib/header.h"

template <typename K, typename V>
class LRUMap {
   public:
    LRUMap(size_t capacity) : capacity_(capacity) {}

    V* find(const K& key) {
        auto toStorage = storage_.find(key);
        if (toStorage != storage_.end()) {
            ASSERT(*(toStorage->second.toListItem_) == key);
            keysOrder_.erase(toStorage->second.toListItem_);
            keysOrder_.emplace_back(key);
            auto last = keysOrder_.end();
            --last;
            toStorage->second.toListItem_ = last;
            return &toStorage->second.value_;
        } else {
            return nullptr;
        }
    }

    void emplace(const K& key, const V& value) {
        ASSERT(storage_.find(key) == storage_.end());

        if (storage_.size() >= capacity_) {
            ASSERT(!keysOrder_.empty());
            auto oldestKey = keysOrder_.front();
            keysOrder_.pop_front();
            auto toKey = storage_.find(oldestKey);
            ASSERT(toKey != storage_.end());
            storage_.erase(toKey);
        }

        ASSERT(storage_.size() + 1 <= capacity_);
        ASSERT(keysOrder_.size() + 1 <= capacity_);

        keysOrder_.emplace_back(key);
        ValueStorage valueStorage;
        valueStorage.value_ = value;
        auto last = keysOrder_.end();
        --last;
        valueStorage.toListItem_ = last;
        storage_.emplace(key, valueStorage);
    }

    size_t size() const {
        return storage_.size();
    }

   private:
    using KeysOrder = std::list<K>;

    struct ValueStorage {
        V value_;
        typename KeysOrder::iterator toListItem_;
    };

    using StorageMap = std::unordered_map<K, ValueStorage>;

    size_t capacity_;
    KeysOrder keysOrder_;
    StorageMap storage_;
};
