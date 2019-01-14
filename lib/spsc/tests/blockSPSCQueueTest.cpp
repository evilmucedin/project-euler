#include "gtest/gtest.h"

#include "lib/spsc/blockSPSCQueue.h"

TEST(BlockSPSCQueue, Simple) {
    BlockSPSCQueue<int> q;
    q.flush();
}

