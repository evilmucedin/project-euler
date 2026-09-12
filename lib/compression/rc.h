#pragma once

#include "lib/header.h"

namespace Rc {

// Higher-ratio LZ codec: LZ77 with a hash-chain match finder and one-step
// lazy parsing, entropy-coded with an adaptive binary range coder
// (LZMA-style bit models). Compared to the byte-oriented Lz block codec it
// compresses considerably tighter (typically 30-40% smaller output on text)
// at a fraction of the speed. Symbol stream per block:
//
//   isMatch bit -> literal (8-bit tree, context = top 3 bits of prev byte)
//   isRep   bit -> match against the last used distance (length only)
//   otherwise   -> match: length coder + distance slot tree + direct bits
//
// Distances are unbounded within a block (the frame layer in lz.h feeds
// 1 MiB blocks). Model state resets per block, so blocks stay independent.
//
// This is a block codec only; for self-describing containers use the frame
// API in lz.h with Level::kHigh.

// Returned on failure (corrupt input, or output that does not fit).
constexpr size_t kError = static_cast<size_t>(-1);

// Scratch size that makes compressBlock never fail for want of space.
size_t compressBound(size_t rawSize);

// Compress src[0..srcSize) into dst. Returns the number of bytes written, or
// kError when the encoded form would exceed dstCapacity (e.g. incompressible
// input with a tight buffer) — the caller should then store the data raw.
size_t compressBlock(const u8* src, size_t srcSize, u8* dst, size_t dstCapacity);

// Decompress a block produced by compressBlock. dstCapacity must be the exact
// original size: the stream carries no terminator and is decoded until the
// output buffer is full. Returns dstCapacity, or kError on corrupt input.
// Never reads outside src[0..srcSize) or writes outside dst[0..dstCapacity).
size_t decompressBlock(const u8* src, size_t srcSize, u8* dst, size_t dstCapacity);

}  // namespace Rc
