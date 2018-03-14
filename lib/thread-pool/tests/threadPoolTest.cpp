#include <functional>
#include <future>
#include <memory>
#include <thread>

#include <gtest/gtest.h>

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

TEST(ThreadPool, notBlockingPostJob) {
    tp::ThreadPool pool;

    std::atomic<int> res(0);

    std::packaged_task<void()> t([&res]() { res += 1; });

    static constexpr size_t kCount = 100000;

    try {
        for (size_t i = 0; i < kCount; ++i) {
            pool.post(t);
        }

        std::cout << res << std::endl;
    } catch (...) {
    }
}

TEST(ThreadPool, blockingPostJob) {
    static constexpr size_t kCount = 100000;

    std::atomic<int> res(0);

    {
        tp::ThreadPool pool;
        std::vector<std::future<void>> futures;

        for (size_t i = 0; i < kCount; ++i) {
            std::packaged_task<void()> t([&res]() { res += 1; });
            futures.emplace_back(t.get_future());

            pool.blockingPost(t);
        }

        for (auto& f: futures) {
            f.get();
        }
    }

    EXPECT_EQ(res, kCount);
}
