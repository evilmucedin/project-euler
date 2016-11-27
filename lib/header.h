#pragma once

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

using u128 = unsigned __int128;
using i128 = __int128;
using u64 = uint64_t;
using i64 = int64_t;
using u32 = uint32_t;
using i32 = int32_t;

using BoolVector = vector<bool>;
using IntVector = vector<int>;
using U64Vector = vector<u64>;
using U128Vector = vector<u128>;

using I32Set = unordered_set<i32>;
using U64Set = unordered_set<u64>;
using U128Set = unordered_set<u128>;

ostream& operator<<(ostream& o, const U64Vector& v);

template<typename T>
typename T::value_type sum(const T& x) {
    typename T::value_type result = 0;
    for (auto v : x) {
        result += v;
    }
    return result;
}
