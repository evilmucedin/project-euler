#include "gtest/gtest.h"

#include "lib/exception.h"
#include "lib/spsc/blockSPSCQueue.h"

TEST(BlockSPSCQueue, Simple) {
    BlockSPSCQueue<int> q;

    std::atomic<int> last{-1};
    std::thread t([&]() {
        int count = 0;
        while (true) {
            int value;
            q.dequeue(value);
            if (value == -1) {
                break;
            }
            if (value != count) {
                THROW("bad value");
            }
            ++count;
        }
        last = count;
    });

    static const size_t N = 1000000;
    for (size_t i = 0; i < N; ++i) {
        q.enqueue(i);
    }

    q.enqueue(-1);

    q.flush();

    t.join();

    EXPECT_EQ(last, N);
}

