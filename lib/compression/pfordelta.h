#pragma once

#include "lib/header.h"

namespace PForDelta {

// Patched Frame-of-Reference (PForDelta) — a simple block-wise integer codec related
// to the family used in search indexes (e.g. Zukowski et al., Lemire & Boytsov).
//
// Per block of up to kBlockSize values:
//   1. Subtract the block minimum (frame of reference) to get deltas.
//   2. Choose a bit width b so that most deltas fit in b bits; outliers are "patched"
//      as (index, full delta) pairs.
//   3. Pack b-bit codes little-endian across bytes; exceptions follow the packed run.
//
// Layout: u32 total_count, then blocks of [ u32 base | u8 b | u8 count | u16
// exception_count | packed ceil(count*b/8) bytes | exceptions... ].
//
// Any u32 sequence is accepted; compression helps most when deltas are small (e.g.
// sorted or locally clustered integers).

constexpr u32 kBlockSize = 128;

// Pack `values` into a byte string. Empty input yields an empty blob.
vector<u8> compress(const U32Vector& values);

// Inverse of compress. On corrupt or truncated input, returns an empty vector.
U32Vector decompress(const vector<u8>& data);

}  // namespace PForDelta
