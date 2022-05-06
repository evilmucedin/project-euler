#include "gtest/gtest.h"
#include "glog/logging.h"

#include "lib/stat/counters.h"

TEST(Counters, ExpDecay) {
    ExpDecayCounter c(10);
    c.bump(1, 1);
    LOG(INFO) << c.get(1);
    LOG(INFO) << c.get(2);
    c.bump(2, 1);
    for (int i = 2; i < 20; ++i) {
        LOG(INFO) << i << "\t" << c.get(i);
    }
}
