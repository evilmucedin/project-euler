#include "threadPool.h"

namespace tp {

/// Implementation

ThreadPoolOptions::ThreadPoolOptions()
    : m_thread_count(std::max<size_t>(1u, std::thread::hardware_concurrency())), m_queue_size(1024u) {}

void ThreadPoolOptions::setThreadCount(size_t count) { m_thread_count = std::max<size_t>(1u, count); }

void ThreadPoolOptions::setQueueSize(size_t size) { m_queue_size = std::max<size_t>(1u, size); }

size_t ThreadPoolOptions::threadCount() const { return m_thread_count; }

size_t ThreadPoolOptions::queueSize() const { return m_queue_size; }
}
