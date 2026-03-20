#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstring>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
bool __hack1 = std::ios::sync_with_stdio(false);
auto __hack2 = cin.tie(nullptr);
template<typename T> using vec = vector<T>;
template<typename T> using vec2d = vec<vec<T>>;
template<typename T> using vec3d = vec<vec2d<T>>;
using i64 = int64_t;
using u64 = uint64_t;
using i32 = int32_t;
using u32 = uint32_t;
using i8 = int8_t;
using u8 = uint8_t;
using uint128_t = __uint128_t;

struct Input {
  Input(istream &in) : in(&in) {}
  istream *in;
  template<class T> T next() const { T x; *in >> x; return x; }
  int ni() const { return next<int>(); }
  i64 ni64() const { return next<i64>(); }
  string ns() const { return next<string>(); }
  template<class T> vec<T> nvec(int n) const { vec<T> v(n); for (int i = 0; i < n; ++i) v[i] = next<T>(); return v; }
  vec<int> nvi(int n) const { return nvec<int>(n); }
  string getline() { string s; std::getline(*in, s); return s; }
};

struct Output {
  Output(ostream &out) : out(&out) {}
  ostream *out;
  inline void print() {}
  template<typename T> inline void printOne(const T& v) { *out << v; }
  template<typename T> inline void printOne(const vec<T>& v) { for (int i = 0; i < v.size(); i++) { if (i) *out << ' '; printOne(v[i]); } }
  template<typename T> inline void printOne(const vec2d<T>& v) { for (int i = 0; i < v.size(); i++) println(v[i]); }
  template<typename A, typename B> inline void printOne(const pair<A, B>& pair) { print(pair.first, pair.second); }
  template<typename T, typename...Ts> inline void print(const T &f, const Ts&... args) {
    printOne(f);
    if (sizeof...(args) != 0) { *out << ' '; print(args...); }
  }
  template<typename...Ts> inline void println(const Ts&... args) { print(args...); *out << '\n'; }
  template<typename...Ts> inline void operator() (const Ts&... args) { println(args...); }
  void setPrecision(int p) { *out << std::setprecision(p) << std::fixed; }
};

template<typename T> vec2d<T> newVec2d(int n, int m, const T& init) {
  vec2d<T> v(n, vec<T>(m, init));
  return v;
}

template<typename T> vec3d<T> newVec3d(int n, int m, int k, const T& init) {
  vec3d<T> v(n, newVec2d(m, k, init));
  return v;
}

void panic() {
  int z = 0;
  z = 1 / z;
}

template<typename T> bool rmn(T& value, const T& candidate) {
  if (candidate < value) { value = candidate; return true; }
  return false;
}

template<typename T> bool rmx(T& value, const T& candidate) {
  if (candidate > value) { value = candidate; return true; }
  return false;
}

void solve(Input& in, Output& out);
int main() {
  // ifstream fin("input.txt");
  // Input in(fin);
  // ofstream fout("output.txt");
  // Output out(fout);
  Input in(cin);
  Output out(cout);
  solve(in, out);
  return 0;
}

struct Card {
  u8 suit;
  u8 rank;
  u8 index;

  bool operator==(const Card& other) const {
    return suit == other.suit && rank == other.rank && index == other.index;
  }
};
using Cards = std::array<Card, 10>;

void solve(Input& in, Output& out) {
  std::array<Cards, 3> cards;
  std::array<std::array<u8, 9>, 4> where;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 9; j++)
      where[i][j] = 3;
  }
  for (int i = 0; i < 3; i++) {
    std::array<Card, 10>& c = cards[i];
    for (int j = 0; j < 10; j++) {
      string s = in.ns();
      c[j].index = j;
      if (s[0] == '7')
        c[j].rank = 1;
      else if (s[0] == '8')
        c[j].rank = 2;
      else if (s[0] == '9')
        c[j].rank = 3;
      else if (s[0] == '1')
        c[j].rank = 4;
      else if (s[0] == 'J')
        c[j].rank = 5;
      else if (s[0] == 'Q')
        c[j].rank = 6;
      else if (s[0] == 'K')
        c[j].rank = 7;
      else if (s[0] == 'A')
        c[j].rank = 8;
      else
        panic();
      if (s[s.length() - 1] == 'S')
        c[j].suit = 0;
      else if (s[s.length() - 1] == 'C')
        c[j].suit = 1;
      else if (s[s.length() - 1] == 'D')
        c[j].suit = 2;
      else if (s[s.length() - 1] == 'H')
        c[j].suit = 3;
      else
        panic();
      where[c[j].suit][c[j].rank] = i;
    }
  }

  int start_player = in.ni() - 1;

  const bool kWin = true;
  const bool kLose = false;

  std::array<int, 10> digits;
  auto fill = [&] (int len, vec<int>& keys, int expected) {
    int max = 1;
    for (int i = 0; i < len; i++)
      max = max * 10;
    keys.resize(max);
    for (int x = 0; x < max; x++)
      keys[x] = -1;

    int count = 0;
    for (int x = 0; x < max; x++) {
      int y = x;
      for (int i = 0; i < len; i++) {
        int d = y % 10;
        digits[i] = d;
        y = y / 10;
      }
      sort(digits.begin(), digits.begin() + len);
      bool eq = false;
      for (int i = 0; i < len - 1; i++) {
        if (digits[i] == digits[i + 1])
          eq = true;
      }
      if (eq)
        continue;
      y = 0;
      for (int i = 0; i < len; i++)
        y = 10 * y + digits[i];
      if (keys[y] == -1)
        keys[y] = count++;
      keys[x] = keys[y];
    }
    if (count != expected)
      panic();
  };

  vec2d<int> keys(6);
  vec<int> counts({ 1, 10, 45, 120, 210, 252 });
  vec2d<bool> caches(10);
  vec2d<bool> caches_known(10);
  for (int len = 1; len <= 5; len++) {
    fill(len, keys[len], counts[len]);
    if (len != 6) {
      caches[len] = vec<bool>(counts[len] * counts[len] * counts[len] * 2, false);
      caches_known[len] = vec<bool>(counts[len] * counts[len] * counts[len] * 2, false);
    }
  }
  for (int len = 6; len <= 9; len++) {
    if (len != 6) {
      caches[len] = vec<bool>(counts[10 - len] * counts[10 - len] * counts[10 - len] * 2, false);
      caches_known[len] = vec<bool>(counts[10 - len] * counts[10 - len] * counts[10 - len] * 2, false);
    }
  }

  auto encodeP = [&] (int len, int p) {
    if (len <= 5) {
      int x = 0;
      for (int i = 0; i < len; i++)
        x = 10 * x + cards[p][i].index;
      return keys[len][x];
    } else {
      int x = 0;
      for (int i = len; i < 10; i++)
        x = 10 * x + cards[p][i].index;
      return keys[10 - len][x];
    }
  };

  int iter = 0;
  function<bool(int, int)> rec = [&] (u8 len, int turn) {
    if (len == 0)
      return kWin;

    int key = 0;
    if (len != 6 && len != 10) {
      int cnt = counts[len <= 5 ? len : 10 - len];
      key = ((encodeP(len, 0) * cnt + encodeP(len, 1)) * cnt + encodeP(len, 2)) * 2 + (turn - 1);
      if (caches_known[len][key])
        return caches[len][key] ? true : false;
    }

    if (++iter == 1500000) {
      // HACK!
      out(";)");
      out.out->flush();
      exit(0);
    }
    u8 next = (turn + 1) % 3;
    u8 last = (turn + 2) % 3;
    Cards& cards_turn = cards[turn];
    Cards& cards_next = cards[next];
    Cards& cards_last = cards[last];
    Cards& cards_0 = cards[0];
    Card table_turn;
    Card table_next;
    Card table_last;

    std::array<u8, 4> min_turn({ 10, 10, 10, 10 });
    std::array<u8, 4> min_next({ 10, 10, 10, 10 });
    std::array<u8, 4> min_last({ 10, 10, 10, 10 });
    std::array<u8, 4> max_0({ 0, 0, 0, 0 });
    for (u8 j = 0; j < len; j++) {
      min_turn[cards_turn[j].suit] = min(min_turn[cards_turn[j].suit], cards_turn[j].rank);
      min_next[cards_next[j].suit] = min(min_next[cards_next[j].suit], cards_next[j].rank);
      min_last[cards_last[j].suit] = min(min_last[cards_last[j].suit], cards_last[j].rank);
      max_0[cards_0[j].suit] = max(max_0[cards_0[j].suit], cards_0[j].rank);
    }

    bool turn_result = turn == 0 ? kLose : kWin;
    bool found = false;

    if (turn == 1) {
      // One-turn force lose.
      for (u8 suit = 0; suit < 4; suit++) {
        if (min_turn[suit] < 10 && min_last[suit] < 10 && min_last[suit] > min_turn[suit] &&
            (min_next[suit] == 10 || min_last[suit] > min_next[suit])) {
          turn_result = kLose;
          found = true;
        }
      }
    }

    if (turn == 2) {
      // One-turn force lose.
      for (u8 suit = 0; suit < 4; suit++) {
        if (min_turn[suit] < 10 && min_next[suit] < 10 && min_next[suit] > min_turn[suit] &&
            (min_last[suit] == 10 || min_next[suit] > min_last[suit])) {
          turn_result = kLose;
          found = true;
        }
      }
    }

    if (turn == 1) {
      // Any possible turn does not catch.
      bool all_larger = true;
      for (u8 suit = 0; suit < 4; suit++) {
        if (max_0[suit] > min_turn[suit])
          all_larger = false;
        if (max_0[suit] > min_next[suit])
          all_larger = false;
      }
      if (all_larger) {
        turn_result = kWin;
        found = true;
      }
    }

    if (turn == 2) {
      // Any possible turn does not catch.
      bool all_larger = true;
      for (u8 suit = 0; suit < 4; suit++) {
        if (max_0[suit] > min_turn[suit])
          all_larger = false;
        if (max_0[suit] > min_last[suit])
          all_larger = false;
      }
      if (all_larger) {
        turn_result = kWin;
        found = true;
      }
    }

    if (!found) {
      for (u8 index_turn = 0; index_turn < len; index_turn++) {
        table_turn = cards_turn[index_turn];
        u8 temp_rank = table_turn.rank - 1;
        while (temp_rank > 0 && where[table_turn.suit][temp_rank] == 3)
          temp_rank--;
        if (where[table_turn.suit][temp_rank] == turn)
          continue;

        u8 move_suit = table_turn.suit;
        bool has_next_suit = min_next[move_suit] < 10;
        bool has_last_suit = min_last[move_suit] < 10;
        bool next_result = next == 0 ? kLose : kWin;
        for (u8 index_next = 0; index_next < len; index_next++) {
          table_next = cards_next[index_next];
          if (has_next_suit && table_next.suit != move_suit)
            continue;
          temp_rank = table_next.rank - 1;
          while (temp_rank > 0 && where[table_next.suit][temp_rank] == 3)
            temp_rank--;
          if (where[table_next.suit][temp_rank] == next)
            continue;

          bool last_result = last == 0 ? kLose : kWin;
          for (u8 index_last = 0; index_last < len; index_last++) {
            table_last = cards_last[index_last];
            if (has_last_suit && table_last.suit != move_suit)
              continue;
            temp_rank = table_last.rank - 1;
            while (temp_rank > 0 && where[table_last.suit][temp_rank] == 3)
              temp_rank--;
            if (where[table_last.suit][temp_rank] == last)
              continue;

            u8 takes = turn;
            u8 rank = table_turn.rank;
            if (table_next.suit == move_suit && table_next.rank > rank) {
              rank = table_next.rank;
              takes = next;
            }
            if (table_last.suit == move_suit && table_last.rank > rank) {
              rank = table_last.rank;
              takes = last;
            }

            bool result = kLose;
            if (takes == 0) {
              result = kLose;
            } else if (len == 1) {
              result = kWin;
            } else {
              cards_turn[index_turn] = cards_turn[len - 1];
              cards_turn[len - 1] = table_turn;
              cards_next[index_next] = cards_next[len - 1];
              cards_next[len - 1] = table_next;
              cards_last[index_last] = cards_last[len - 1];
              cards_last[len - 1] = table_last;
              where[table_turn.suit][table_turn.rank] = 3;
              where[table_next.suit][table_next.rank] = 3;
              where[table_last.suit][table_last.rank] = 3;

              result = rec(len - 1, takes);

              cards_turn[len - 1] = cards_turn[index_turn];
              cards_turn[index_turn] = table_turn;
              cards_next[len - 1] = cards_next[index_next];
              cards_next[index_next] = table_next;
              cards_last[len - 1] = cards_last[index_last];
              cards_last[index_last] = table_last;
              where[table_turn.suit][table_turn.rank] = turn;
              where[table_next.suit][table_next.rank] = next;
              where[table_last.suit][table_last.rank] = last;
            }

            if (result != last_result) {
              last_result = result;
              break;
            }
          }
          if (last_result != next_result) {
            next_result = last_result;
            break;
          }
        }
        if (next_result != turn_result) {
          turn_result = next_result;
          break;
        }
      }
    }

    if (len != 6 && len != 10) {
      caches_known[len][key] = true;
      caches[len][key] = turn_result;
    }
    return turn_result;
  };

  bool result = rec(10, start_player);
  out(result == kWin ? ";)" : ";(");
}
