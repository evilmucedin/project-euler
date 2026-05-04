#include "lib/header.h"

#include "lib/random.h"
#include "lib/timer.h"

#include "lib/robinHood.h"

#include "gtest/gtest.h"

template<typename Map>
void fillMap(const StringVector& keys, const IntVector& values, Map&& map) {
    Timer t("fill map");
    for (size_t i = 0; i < keys.size(); ++i) {
        map.emplace(keys[i], values[i]);
    }
}

TEST(robinHood, Benchmark) {
    static constexpr size_t N = 1000000;
    StringVector keys(N);
    IntVector values(N);
    REP (i, N) {
        keys[i] = randString(10);
        values[i] = randInt();
    }

    fillMap(keys, values, robin_hood::unordered_map<string, int>());
    fillMap(keys, values, std::unordered_map<string, int>());
}
