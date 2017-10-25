#include "lib/header.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <unistd.h>

class Semaphore
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_ = 0; // Initialized as locked.

public:
    Semaphore(unsigned long count) {
        count_ = count;
    }

    void notify(int count = 1) {
        std::unique_lock<decltype(mutex_)> lock(mutex_);
        count_ += count;
        condition_.notify_one();
    }

    void wait(int count = 1) {
        std::unique_lock<decltype(mutex_)> lock(mutex_);
        while (count_ < count) // Handle spurious wake-ups.
            condition_.wait(lock);
        count_ -= count;
    }
};


class ReadWriteLock {
private:
    static constexpr size_t kMaxReads = 1000;

public:
    ReadWriteLock()
        : s_(kMaxReads)
    {
    }

    void acquireReadLock() {
        s_.wait();
    }

    void releaseReadLock() {
        s_.notify();
    }

    void acquireWriteLock() {
        s_.wait(kMaxReads);
    }

    void releaseWriteLock() {
        s_.notify(kMaxReads);
    }

private:
    Semaphore s_;
};

static constexpr size_t kThreads = 16;

void testOneWrite() {
    atomic<int> readCounter(0);
    atomic<int> writeCounter(0);
    atomic<int> maxReadCounter(0);
    vector<thread> threads(kThreads);
    static constexpr size_t nTries = 10000;
    ReadWriteLock rw;
    for (size_t i = 0; i < kThreads; ++i) {
        threads[i] = thread([&]() {
            for (size_t j  = 0; j < nTries; ++j) {
                if (rand() & 1) {
                    rw.acquireWriteLock();
                    if (0 != readCounter) {
                        cout << "BAD" << endl;
                        throw exception();
                    }
                    if (0 != writeCounter) {
                        cout << "BAD" << endl;
                        throw exception();
                    }
                    ++writeCounter;
                    if (1 != writeCounter) {
                        cout << "BAD" << endl;
                        throw exception();
                    }
                    sleep(0);
                    if (0 != readCounter) {
                        cout << "BAD" << endl;
                        throw exception();
                    }
                    if (1 != writeCounter) {
                        cout << "BAD" << endl;
                        throw exception();
                    }
                    --writeCounter;
                    if (0 != writeCounter) {
                        cout << "BAD" << endl;
                        throw exception();
                    }
                    rw.releaseWriteLock();
                } else {
                    rw.acquireReadLock();
                    if (0 != writeCounter) {
                        cout << "BAD" << endl;
                        throw exception();
                    }
                    ++readCounter;
                    if (readCounter > maxReadCounter) {
                        maxReadCounter.store(readCounter);
                    }
                    sleep(0);
                    --readCounter;
                    if (0 != writeCounter) {
                        cout << "BAD" << endl;
                        throw exception();
                    }
                    rw.releaseReadLock();
                }
            }
        });
    }
    for (auto& t: threads) {
        t.join();
    }
    cout << "ReadCounter: " << readCounter << " WriteCounter: " << writeCounter << " MaxReadCounter: " << maxReadCounter << endl;
};

int main() {
    testOneWrite();

    return 0;
}
