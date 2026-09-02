#pragma once

#include "lib/header.h"

namespace Lz {

// Fast general-purpose byte-oriented LZ codec (LZ4-family block format).
//
// A block is a sequence of tokens: [ u8 token | literals | u16 offset | ... ].
// The token's high nibble is the literal count and the low nibble the match
// length minus 4, each extended by 255-continuation bytes when it saturates.
// Matches reference up to 64 KiB back, so compression speed is dominated by a
// single multiplicative hash lookup per position and decompression is a
// straight alternation of memcpy and overlap copies.
//
// Two layers:
//   * Block API — raw block in a caller-provided buffer, no header, the caller
//     must remember the uncompressed size.
//   * Frame API — self-describing container used by the in-memory helpers and
//     the file helpers: a magic word, 1 MiB blocks each preceded by
//     [ u32 rawSize | u32 compressedSize ] (compressedSize == 0 means the
//     block is stored uncompressed), a rawSize == 0 terminator block, and a
//     u64 checksum of the original data. Incompressible input therefore costs
//     ~8 bytes per MiB plus the fixed 16-byte envelope.

// ---------------------------------------------------------------------------
// Block API.

// Returned by decompressBlock on corrupt input.
constexpr size_t kError = static_cast<size_t>(-1);

// Worst-case compressed size of a block of rawSize bytes.
size_t compressBound(size_t rawSize);

// Compress src[0..srcSize) into dst. Returns the number of bytes written, or
// kError when dstCapacity < compressBound(srcSize).
size_t compressBlock(const u8* src, size_t srcSize, u8* dst, size_t dstCapacity);

// Decompress an exact block produced by compressBlock. Returns the number of
// bytes written to dst, or kError on corrupt input or insufficient capacity.
// Never reads outside src[0..srcSize) or writes outside dst[0..dstCapacity).
size_t decompressBlock(const u8* src, size_t srcSize, u8* dst, size_t dstCapacity);

// Fast 64-bit hash (Murmur3-style) used as the frame checksum.
u64 hash64(const void* data, size_t size);

// ---------------------------------------------------------------------------
// Frame API — in memory.

// Compress into a self-describing checksummed frame.
vector<u8> compress(const void* data, size_t size);
vector<u8> compress(const vector<u8>& data);
vector<u8> compress(const string& data);

// Decompress a frame. Returns false on corrupt, truncated, or checksum-
// mismatching input, leaving out in an unspecified state.
bool decompress(const u8* data, size_t size, vector<u8>& out);

// Convenience wrapper: returns an empty vector on error (indistinguishable
// from a genuinely empty payload — use the bool overload when that matters).
vector<u8> decompress(const vector<u8>& data);

// ---------------------------------------------------------------------------
// Frame API — on disk. Streams 1 MiB blocks, memory use is O(block), not
// O(file). Return false on I/O error or (for decompressFile) corrupt input.

bool compressFile(const string& inputPath, const string& outputPath);
bool decompressFile(const string& inputPath, const string& outputPath);

}  // namespace Lz
