#include <gtest/gtest.h>

#include <chrono>
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
}  // namespace TestLinkage

TEST(ThreadPool, fixedFunction) {
    int res = 0;
    std::packaged_task<void()> t([&res]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        res = 42;
    });

    tp::FixedFunction<void(), 128> task(t);
    task();
    // t();
    // t.get_future().get();

    ASSERT_EQ(42, res);
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
    for (size_t count : {13, 111113}) {
        static constexpr size_t kTests = 5;

        std::atomic<int> res(0);
        double sumDelay = 0;
        double maxDelay = 0;

        {
            tp::ThreadPool pool;

            for (size_t j = 0; j < kTests; ++j) {
                std::vector<std::future<double>> futures;

                for (size_t i = 0; i < count; ++i) {
                    auto create = std::chrono::high_resolution_clock::now();
                    std::packaged_task<double()> t([&res, create]() {
                        res += 1;
                        return std::chrono::duration_cast<std::chrono::microseconds>(
                                   std::chrono::high_resolution_clock::now() - create)
                            .count();
                    });
                    futures.emplace_back(t.get_future());

                    pool.blockingPost(t);
                }

                for (auto& f : futures) {
                    auto d = f.get();
                    sumDelay += d;
                    maxDelay = std::max(maxDelay, d);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(497));
            }
        }

        EXPECT_EQ(res, count * kTests);
        std::cout << "Count: " << count << ", avg. delay: " << sumDelay / count / kTests << ", max delay: " << maxDelay
                  << std::endl;
    }
}
