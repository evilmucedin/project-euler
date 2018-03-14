#include <gtest/gtest.h>

#include <functional>
#include <future>
#include <memory>
#include <thread>

#include "lib/thread-pool/threadPool.h"

namespace TestLinkage {
size_t getWorkerIdForCurrentThread() { return *tp::detail::thread_id(); }

size_t getWorkerIdForCurrentThread2() {
    return tp::Worker<std::function<void()>, tp::MPMCBoundedQueue>::getWorkerIdForCurrentThread();
}
}

TEST(ThreadPool, postJob) {
    tp::ThreadPool pool;

    std::packaged_task<int()> t([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return 42;
    });

    std::future<int> r = t.get_future();

    pool.post(t);

    ASSERT_EQ(42, r.get());
}
