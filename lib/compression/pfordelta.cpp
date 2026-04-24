#include "lib/compression/pfordelta.h"

namespace PForDelta {

namespace {

void appendU32(vector<u8>& out, u32 v) {
  out.push_back(static_cast<u8>(v & 0xff));
  out.push_back(static_cast<u8>((v >> 8) & 0xff));
  out.push_back(static_cast<u8>((v >> 16) & 0xff));
  out.push_back(static_cast<u8>((v >> 24) & 0xff));
}

bool readU32(const vector<u8>& in, u64& off, u32& v) {
  if (off + 4 > in.size()) return false;
  v = static_cast<u32>(in[off]) | (static_cast<u32>(in[off + 1]) << 8) |
      (static_cast<u32>(in[off + 2]) << 16) | (static_cast<u32>(in[off + 3]) << 24);
  off += 4;
  return true;
}

void appendBits(vector<u8>& out, u64& bit_off, u32 value, int b) {
  for (int i = 0; i < b; ++i) {
    const size_t byte_idx = static_cast<size_t>(bit_off >> 3);
    const int bit_in_byte = static_cast<int>(bit_off & 7);
    if (byte_idx >= out.size()) out.push_back(0);
    if (value & (1u << i)) out[byte_idx] |= static_cast<u8>(1u << bit_in_byte);
    ++bit_off;
  }
}

bool readBits(const vector<u8>& in, u64& bit_off, int b, u32& out_val) {
  out_val = 0;
  for (int i = 0; i < b; ++i) {
    const size_t byte_idx = static_cast<size_t>(bit_off >> 3);
    const int bit_in_byte = static_cast<int>(bit_off & 7);
    if (byte_idx >= in.size()) return false;
    if (in[byte_idx] & (1u << bit_in_byte)) out_val |= (1u << i);
    ++bit_off;
  }
  return true;
}

inline bool needsException(u32 delta, int b) {
  if (b >= 32) return false;
  return delta >= (1u << b);
}

// Pick b in [0,32] minimizing packed size + exception overhead (1 byte index + 4
// byte delta per exception).
int chooseBitWidth(const vector<u32>& deltas) {
  const size_t n = deltas.size();
  size_t best_cost = static_cast<size_t>(-1);
  int best_b = 32;
  for (int b = 0; b <= 32; ++b) {
    size_t exc = 0;
    for (u32 d : deltas) {
      if (needsException(d, b)) ++exc;
    }
    const size_t packed_bits = n * static_cast<size_t>(b);
    const size_t packed_bytes = (packed_bits + 7) / 8;
    const size_t cost = packed_bytes + exc * (1 + 4);
    if (cost < best_cost) {
      best_cost = cost;
      best_b = b;
    }
  }
  return best_b;
}

}  // namespace

vector<u8> compress(const U32Vector& values) {
  vector<u8> out;
  const u32 n = static_cast<u32>(values.size());
  appendU32(out, n);
  if (n == 0) return out;

  size_t pos = 0;
  while (pos < values.size()) {
    const u8 count =
        static_cast<u8>(min<u64>(kBlockSize, values.size() - pos));
    u32 base = values[pos];
    for (u8 i = 1; i < count; ++i) base = min(base, values[pos + i]);

    vector<u32> deltas(count);
    for (u8 i = 0; i < count; ++i) deltas[i] = values[pos + i] - base;

    const int b = chooseBitWidth(deltas);

    vector<pair<u8, u32>> exceptions;
    exceptions.reserve(count);
    for (u8 i = 0; i < count; ++i) {
      if (needsException(deltas[i], b)) exceptions.emplace_back(i, deltas[i]);
    }

    appendU32(out, base);
    out.push_back(static_cast<u8>(b));
    out.push_back(count);
    out.push_back(static_cast<u8>(exceptions.size() & 0xff));
    out.push_back(static_cast<u8>((exceptions.size() >> 8) & 0xff));

    const size_t packed_start = out.size();
    const u64 total_bits = static_cast<u64>(count) * static_cast<u64>(b);
    const size_t packed_bytes = static_cast<size_t>((total_bits + 7) / 8);
    out.resize(packed_start + packed_bytes, 0);
    u64 bit_off = static_cast<u64>(packed_start) * 8;

    for (u8 i = 0; i < count; ++i) {
      const u32 d = deltas[i];
      const u32 pack_val = needsException(d, b) ? 0u : d;
      appendBits(out, bit_off, pack_val, b);
    }

    for (const auto& ex : exceptions) {
      out.push_back(ex.first);
      appendU32(out, ex.second);
    }

    pos += count;
  }
  return out;
}

U32Vector decompress(const vector<u8>& data) {
  U32Vector values;
  u64 off = 0;
  u32 n = 0;
  if (!readU32(data, off, n)) return {};
  values.reserve(n);
  if (n == 0) return values;

  while (values.size() < n) {
    u32 base = 0;
    if (!readU32(data, off, base)) return {};
    if (off + 4 > data.size()) return {};
    const int b = static_cast<int>(data[off++]);
    const u8 count = data[off++];
    const u16 exc_count =
        static_cast<u16>(data[off]) | (static_cast<u16>(data[off + 1]) << 8);
    off += 2;
    if (count == 0 || values.size() + count > n) return {};

    u64 bit_off = off * 8;
    vector<u32> deltas(count);
    for (u8 i = 0; i < count; ++i) {
      u32 low = 0;
      if (!readBits(data, bit_off, b, low)) return {};
      deltas[i] = low;
    }
    off = (bit_off + 7) / 8;

    for (u16 e = 0; e < exc_count; ++e) {
      if (off + 5 > data.size()) return {};
      const u8 idx = data[off++];
      u32 full = 0;
      if (!readU32(data, off, full)) return {};
      if (idx >= count) return {};
      deltas[idx] = full;
    }

    for (u8 i = 0; i < count; ++i) values.push_back(base + deltas[i]);
  }

  if (values.size() != n) return {};
  return values;
}

}  // namespace PForDelta
