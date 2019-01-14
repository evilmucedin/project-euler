#pragma once

#include <memory>
#include <thread>

#include "moodycamel/readerwriterqueue.h"

template <typename T, size_t BLOCK_SIZE>
class RingBlock {
   public:
    bool empty() const { return front_ == back_; }

    size_t size() const { return (back_ + BLOCK_SIZE - front_) % BLOCK_SIZE; }

    void push_back(const T& value) {
        data_[back_] = value;
        back_ = (back_ + 1) % BLOCK_SIZE;
    }

    void emplace_back(T&& value) {
        data_[back_] = std::move(value);
        back_ = (back_ + 1) % BLOCK_SIZE;
    }

    T pop_front() {
        T result = data_[front_];
        front_ = (front_ + 1) % BLOCK_SIZE;
        return result;
    }

   private:
    size_t front_{};
    size_t back_{};
    std::array<T, BLOCK_SIZE> data_;
};

template <typename T, size_t BLOCK_SIZE = 64>
class BlockSPSCQueue {
   public:
    void enqueue(const T& value) {
        if (!back_block_) {
            back_block_ = getBlock();
        }
        back_block_->push_back(value);
        if (back_block_->size() + 1 == BLOCK_SIZE) {
            flush();
        }
    }

    void enqueue(T&& value) {
        if (!back_block_) {
            back_block_ = getBlock();
        }
        back_block_->emplace_back(std::move(value));
        if (back_block_->size() + 1 == BLOCK_SIZE) {
            flush();
        }
    }

    void dequeue(T& result) {
        if (!front_block_) {
            q_.wait_dequeue(front_block_);
        }
        result = front_block_->pop_front();
        if (front_block_->empty()) {
            empty_.try_enqueue(front_block_);
            front_block_.reset();
        }
    }

    void flush() {
        if (back_block_ && !back_block_->empty()) {
            while (!q_.enqueue(back_block_)) {
                yield();
            }
            back_block_.reset();
        }
    }

   private:
    void yield() { std::this_thread::sleep_for(std::chrono::microseconds(1)); }

    using Storage = RingBlock<T, BLOCK_SIZE>;
    using PStorage = std::shared_ptr<Storage>;

    PStorage getBlock() {
        PStorage result;
        if (!empty_.try_dequeue(result)) {
            result = std::make_shared<Storage>();
        }
        return result;
    }

    moodycamel::BlockingReaderWriterQueue<PStorage> q_;
    moodycamel::BlockingReaderWriterQueue<PStorage> empty_;
    PStorage front_block_;
    PStorage back_block_;
};
