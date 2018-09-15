#include "threadPool.h"

namespace tp {

/// Implementation

ThreadPoolOptions::ThreadPoolOptions()
    : thread_count_(std::max<size_t>(1u, std::thread::hardware_concurrency())), queue_size_(1024u) {}

void ThreadPoolOptions::setThreadCount(size_t count) { thread_count_ = std::max<size_t>(1u, count); }

void ThreadPoolOptions::setQueueSize(size_t size) { queue_size_ = std::max<size_t>(1u, size); }

size_t ThreadPoolOptions::threadCount() const { return thread_count_; }

size_t ThreadPoolOptions::queueSize() const { return queue_size_; }
}
