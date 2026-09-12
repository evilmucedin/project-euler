#include "lib/compression/rc.h"

#include <cstring>

namespace Rc {

namespace {

// Range coder parameters (classic LZMA values).
constexpr u32 kProbBits = 11;                        // probabilities in [0, 2048)
constexpr u16 kProbInit = 1 << (kProbBits - 1);      // p(bit=0) = 1/2
constexpr int kMoveBits = 5;                         // adaptation speed
constexpr u32 kTopValue = 1u << 24;

// Parse parameters.
constexpr size_t kMinMatch = 2;                      // shortest encodable match
constexpr size_t kMaxLen = kMinMatch + 8 + 8 + 255;  // length coder capacity
constexpr int kHash4Log = 17;
constexpr int kHash3Log = 14;
constexpr int kMaxChainDepth = 64;
constexpr size_t kMaxShortDist = 4096;  // a 3-byte match must be at least this close

u32 readU32(const u8* p) {
  u32 v;
  memcpy(&v, p, sizeof(v));
  return v;
}

u64 readU64(const u8* p) {
  u64 v;
  memcpy(&v, p, sizeof(v));
  return v;
}

u32 hash4(const u8* p) { return (readU32(p) * 2654435761u) >> (32 - kHash4Log); }

u32 hash3(const u8* p) {
  const u32 v = p[0] | (static_cast<u32>(p[1]) << 8) | (static_cast<u32>(p[2]) << 16);
  return (v * 2654435761u) >> (32 - kHash3Log);
}

int highestBit(u32 v) {
#if defined(__GNUC__) || defined(__clang__)
  return 31 - __builtin_clz(v);
#else
  int b = 0;
  while (v >>= 1) ++b;
  return b;
#endif
}

// Number of leading bytes equal between the two ranges, not reading past limit.
size_t matchLength(const u8* p, const u8* q, const u8* limit) {
  const u8* start = p;
  while (p + sizeof(u64) <= limit) {
    u64 diff = readU64(p) ^ readU64(q);
    if (diff) {
#if defined(__GNUC__) || defined(__clang__)
      return static_cast<size_t>(p - start) + (__builtin_ctzll(diff) >> 3);
#else
      break;
#endif
    }
    p += sizeof(u64);
    q += sizeof(u64);
  }
  while (p < limit && *p == *q) {
    ++p;
    ++q;
  }
  return static_cast<size_t>(p - start);
}

// ---------------------------------------------------------------------------
// Adaptive binary range coder.

struct Encoder {
  u8* out;
  u8* const outEnd;
  u64 low = 0;
  u32 range = 0xffffffffu;
  u8 cache = 0;
  u64 cacheSize = 1;
  bool overflow = false;

  void put(u8 b) {
    if (out == outEnd) {
      overflow = true;
      return;
    }
    *out++ = b;
  }

  void shiftLow() {
    if (static_cast<u32>(low) < 0xff000000u || (low >> 32) != 0) {
      const u8 carry = static_cast<u8>(low >> 32);
      u8 b = cache;
      do {
        put(static_cast<u8>(b + carry));
        b = 0xff;
      } while (--cacheSize);
      cache = static_cast<u8>(low >> 24);
    }
    ++cacheSize;
    // Keep only bits 0..23 (bits 24..31 just moved to cache, bit 32 was the
    // carry): the shift must truncate in 32-bit arithmetic.
    low = static_cast<u32>(static_cast<u32>(low) << 8);
  }

  void encodeBit(u16& prob, u32 bit) {
    const u32 bound = (range >> kProbBits) * prob;
    if (bit == 0) {
      range = bound;
      prob += ((1u << kProbBits) - prob) >> kMoveBits;
    } else {
      low += bound;
      range -= bound;
      prob -= prob >> kMoveBits;
    }
    while (range < kTopValue) {
      range <<= 8;
      shiftLow();
    }
  }

  void encodeDirect(u32 value, int numBits) {
    for (int i = numBits - 1; i >= 0; --i) {
      range >>= 1;
      if ((value >> i) & 1) {
        low += range;
      }
      while (range < kTopValue) {
        range <<= 8;
        shiftLow();
      }
    }
  }

  template <int NumBits>
  void encodeTree(u16* probs, u32 symbol) {
    u32 m = 1;
    for (int i = NumBits - 1; i >= 0; --i) {
      const u32 bit = (symbol >> i) & 1;
      encodeBit(probs[m], bit);
      m = (m << 1) | bit;
    }
  }

  void flush() {
    for (int i = 0; i < 5; ++i) {
      shiftLow();
    }
  }
};

struct RangeDecoder {
  const u8* in;
  const u8* const inEnd;
  u32 range = 0xffffffffu;
  u32 code = 0;
  bool fail = false;

  u8 next() {
    if (in == inEnd) {
      // Corrupt/truncated stream; keep decoding zeros so the caller's output
      // bound terminates the loop, then report failure.
      fail = true;
      return 0;
    }
    return *in++;
  }

  void init() {
    for (int i = 0; i < 5; ++i) {  // first byte is the encoder's zero cache
      code = (code << 8) | next();
    }
  }

  u32 decodeBit(u16& prob) {
    const u32 bound = (range >> kProbBits) * prob;
    u32 bit;
    if (code < bound) {
      range = bound;
      prob += ((1u << kProbBits) - prob) >> kMoveBits;
      bit = 0;
    } else {
      code -= bound;
      range -= bound;
      prob -= prob >> kMoveBits;
      bit = 1;
    }
    while (range < kTopValue) {
      range <<= 8;
      code = (code << 8) | next();
    }
    return bit;
  }

  u32 decodeDirect(int numBits) {
    u32 result = 0;
    for (int i = 0; i < numBits; ++i) {
      range >>= 1;
      code -= range;
      const u32 t = 0 - (code >> 31);  // all-ones iff the subtraction borrowed
      code += range & t;
      result = (result << 1) + (t + 1);
      while (range < kTopValue) {
        range <<= 8;
        code = (code << 8) | next();
      }
    }
    return result;
  }

  template <int NumBits>
  u32 decodeTree(u16* probs) {
    u32 m = 1;
    for (int i = 0; i < NumBits; ++i) {
      m = (m << 1) | decodeBit(probs[m]);
    }
    return m - (1u << NumBits);
  }
};

// ---------------------------------------------------------------------------
// Symbol models. All probabilities start at 1/2 and reset per block.

struct LenModel {
  // Length code v = len - kMinMatch: 0..7 in `low`, 8..15 in `mid`,
  // 16..271 in `high`, selected by choice/choice2 bits.
  u16 choice = kProbInit;
  u16 choice2 = kProbInit;
  u16 low[8];
  u16 mid[8];
  u16 high[256];

  LenModel() {
    fill(begin(low), end(low), kProbInit);
    fill(begin(mid), end(mid), kProbInit);
    fill(begin(high), end(high), kProbInit);
  }
};

void encodeLen(Encoder& rc, LenModel& m, u32 v) {
  if (v < 8) {
    rc.encodeBit(m.choice, 0);
    rc.encodeTree<3>(m.low, v);
  } else if (v < 16) {
    rc.encodeBit(m.choice, 1);
    rc.encodeBit(m.choice2, 0);
    rc.encodeTree<3>(m.mid, v - 8);
  } else {
    rc.encodeBit(m.choice, 1);
    rc.encodeBit(m.choice2, 1);
    rc.encodeTree<8>(m.high, v - 16);
  }
}

u32 decodeLen(RangeDecoder& rc, LenModel& m) {
  if (rc.decodeBit(m.choice) == 0) {
    return rc.decodeTree<3>(m.low);
  }
  if (rc.decodeBit(m.choice2) == 0) {
    return 8 + rc.decodeTree<3>(m.mid);
  }
  return 16 + rc.decodeTree<8>(m.high);
}

struct Models {
  u16 isMatch[2];         // context: was the previous symbol a match?
  u16 isRep[2];
  u16 literal[8][256];    // 8-bit trees, context = top 3 bits of previous byte
  LenModel matchLen;
  LenModel repLen;
  u16 slot[4][64];        // distance slot trees, context = min(lenCode, 3)

  Models() {
    fill(begin(isMatch), end(isMatch), kProbInit);
    fill(begin(isRep), end(isRep), kProbInit);
    fill(&literal[0][0], &literal[0][0] + 8 * 256, kProbInit);
    fill(&slot[0][0], &slot[0][0] + 4 * 64, kProbInit);
  }
};

// Distance is transmitted as d = dist - 1: values 0..3 are their own slot;
// larger values use slot = (log2(d) << 1) | next-highest-bit, with the
// remaining low bits sent as uniform direct bits.

void encodeDist(Encoder& rc, Models& m, u32 lenCode, u32 d) {
  u32 slotVal = d;
  if (d >= 4) {
    const int nb = highestBit(d);
    slotVal = (static_cast<u32>(nb) << 1) | ((d >> (nb - 1)) & 1);
  }
  rc.encodeTree<6>(m.slot[min(lenCode, 3u)], slotVal);
  if (slotVal >= 4) {
    const int footerBits = static_cast<int>(slotVal >> 1) - 1;
    rc.encodeDirect(d & ((1u << footerBits) - 1), footerBits);
  }
}

u32 decodeDist(RangeDecoder& rc, Models& m, u32 lenCode) {
  const u32 slotVal = rc.decodeTree<6>(m.slot[min(lenCode, 3u)]);
  if (slotVal < 4) {
    return slotVal;
  }
  const int footerBits = static_cast<int>(slotVal >> 1) - 1;
  return ((2u | (slotVal & 1)) << footerBits) | rc.decodeDirect(footerBits);
}

}  // namespace

size_t compressBound(size_t rawSize) {
  // Adaptive coding of incompressible data costs a hair over 8 bits/byte,
  // plus the coder's fixed lead-in/flush bytes.
  return rawSize + rawSize / 16 + 64;
}

size_t compressBlock(const u8* src, size_t srcSize, u8* dst, size_t dstCapacity) {
  Encoder rc{dst, dst + dstCapacity};
  Models models;
  U32Vector head4(size_t(1) << kHash4Log, 0);
  U32Vector head3(size_t(1) << kHash3Log, 0);
  U32Vector prev(srcSize, 0);  // hash chains; positions stored +1, 0 = empty
  size_t rep0 = 1;
  bool prevWasMatch = false;
  u8 prevByte = 0;

  auto insert = [&](size_t p) {
    if (p + 4 > srcSize) {
      return;
    }
    const u32 h = hash4(src + p);
    prev[p] = head4[h];
    head4[h] = static_cast<u32>(p) + 1;
    head3[hash3(src + p)] = static_cast<u32>(p) + 1;
  };

  struct Cand {
    size_t len = 0;
    size_t dist = 0;
  };

  // Best match at p: hash-chain search (min length 3, near-only for length 3)
  // and a match at the last used distance (min length 2, preferred when it is
  // at most one byte shorter — it costs no distance bits).
  auto search = [&](size_t p) -> Cand {
    const size_t maxLen = min(kMaxLen, srcSize - p);
    if (maxLen < kMinMatch) {
      return {};
    }
    const u8* const cur = src + p;
    const u8* const limit = cur + maxLen;
    Cand best;
    if (maxLen >= 4) {
      u32 node = head4[hash4(cur)];
      for (int depth = kMaxChainDepth; node != 0 && depth != 0; --depth) {
        const size_t cand = node - 1;
        const u8* cp = src + cand;
        if (cp[best.len] == cur[best.len]) {
          const size_t l = matchLength(cur, cp, limit);
          if (l > best.len) {
            best = {l, p - cand};
            if (l == maxLen) {
              break;
            }
          }
        }
        node = prev[cand];
      }
    }
    if (best.len < 3 && maxLen >= 3 && head3[hash3(cur)] != 0) {
      const size_t cand = head3[hash3(cur)] - 1;
      const size_t l = matchLength(cur, src + cand, limit);
      if (l >= 3 && l > best.len) {
        best = {l, p - cand};
      }
    }
    if (best.len == 3 && best.dist > kMaxShortDist) {
      best.len = 0;  // a distant 3-byte match costs more than 3 literals
    }
    if (rep0 <= p) {
      const size_t repLen = matchLength(cur, cur - rep0, limit);
      if (repLen >= kMinMatch && repLen + 1 >= best.len) {
        return {repLen, rep0};
      }
    }
    return best.len >= 3 ? best : Cand{};
  };

  auto encodeLiteral = [&](size_t p) {
    rc.encodeBit(models.isMatch[prevWasMatch], 0);
    rc.encodeTree<8>(models.literal[prevByte >> 5], src[p]);
    prevWasMatch = false;
    prevByte = src[p];
  };

  auto encodeMatch = [&](size_t p, size_t len, size_t dist) {
    rc.encodeBit(models.isMatch[prevWasMatch], 1);
    const u32 lenCode = static_cast<u32>(len - kMinMatch);
    if (dist == rep0) {
      rc.encodeBit(models.isRep[prevWasMatch], 1);
      encodeLen(rc, models.repLen, lenCode);
    } else {
      rc.encodeBit(models.isRep[prevWasMatch], 0);
      encodeLen(rc, models.matchLen, lenCode);
      encodeDist(rc, models, lenCode, static_cast<u32>(dist - 1));
      rep0 = dist;
    }
    prevWasMatch = true;
    prevByte = src[p + len - 1];
  };

  // Greedy parse with one-step lazy matching: a match found at `pos` is held
  // for one position, and dropped to a literal if `pos + 1` finds a longer one.
  size_t pos = 0;
  size_t holdLen = 0;
  size_t holdDist = 0;  // pending match at pos - 1
  while (pos < srcSize) {
    const Cand c = search(pos);
    insert(pos);
    if (holdLen != 0) {
      if (c.len > holdLen) {
        encodeLiteral(pos - 1);
        holdLen = c.len;
        holdDist = c.dist;
        ++pos;
      } else {
        const size_t end = pos - 1 + holdLen;
        encodeMatch(pos - 1, holdLen, holdDist);
        for (size_t q = pos + 1; q < end; ++q) {
          insert(q);
        }
        pos = end;
        holdLen = 0;
      }
    } else if (c.len != 0) {
      holdLen = c.len;
      holdDist = c.dist;
      ++pos;
    } else {
      encodeLiteral(pos);
      ++pos;
    }
  }
  if (holdLen != 0) {
    encodeMatch(pos - 1, holdLen, holdDist);
  }
  rc.flush();
  return rc.overflow ? kError : static_cast<size_t>(rc.out - dst);
}

size_t decompressBlock(const u8* src, size_t srcSize, u8* dst, size_t dstCapacity) {
  RangeDecoder rc{src, src + srcSize};
  rc.init();
  Models models;
  size_t rep0 = 1;
  bool prevWasMatch = false;
  u8 prevByte = 0;
  u8* op = dst;
  u8* const oend = dst + dstCapacity;

  while (op < oend) {
    if (rc.decodeBit(models.isMatch[prevWasMatch]) == 0) {
      const u8 sym = static_cast<u8>(rc.decodeTree<8>(models.literal[prevByte >> 5]));
      *op++ = sym;
      prevByte = sym;
      prevWasMatch = false;
      continue;
    }
    size_t len;
    size_t dist;
    if (rc.decodeBit(models.isRep[prevWasMatch]) != 0) {
      len = kMinMatch + decodeLen(rc, models.repLen);
      dist = rep0;
    } else {
      const u32 lenCode = decodeLen(rc, models.matchLen);
      len = kMinMatch + lenCode;
      dist = static_cast<size_t>(decodeDist(rc, models, lenCode)) + 1;
      rep0 = dist;
    }
    if (rc.fail || dist > static_cast<size_t>(op - dst) ||
        len > static_cast<size_t>(oend - op)) {
      return kError;
    }
    const u8* match = op - dist;
    if (dist >= sizeof(u64)) {
      size_t n = len;
      while (n >= sizeof(u64)) {
        memcpy(op, match, sizeof(u64));
        op += sizeof(u64);
        match += sizeof(u64);
        n -= sizeof(u64);
      }
      while (n--) {
        *op++ = *match++;
      }
    } else {
      for (size_t i = 0; i < len; ++i) {
        *op++ = *match++;
      }
    }
    prevByte = op[-1];
    prevWasMatch = true;
  }
  return rc.fail ? kError : static_cast<size_t>(op - dst);
}

}  // namespace Rc
