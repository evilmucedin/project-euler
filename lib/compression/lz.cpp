#include "lib/compression/lz.h"

#include <cstdio>
#include <cstring>

#include "lib/compression/rc.h"

namespace Lz {

namespace {

// Block format constants (LZ4-family).
constexpr size_t kMinMatch = 4;         // shortest encodable match
constexpr size_t kLastLiterals = 5;     // final bytes are always literals
constexpr size_t kMfLimit = 12;         // no match may start this close to the end
constexpr size_t kMaxOffset = 0xffff;   // 16-bit match offsets
constexpr int kHashLog = 16;            // 64 K-entry match table
constexpr int kSkipTrigger = 6;         // acceleration: step grows every 2^6 misses

// Frame constants.
constexpr u32 kMagic = 0x315a4c44;               // "DLZ1" little-endian
constexpr size_t kFrameBlockSize = 1u << 20;     // 1 MiB per frame block

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

void writeU16(u8* p, u16 v) { memcpy(p, &v, sizeof(v)); }
void writeU32(u8* p, u32 v) { memcpy(p, &v, sizeof(v)); }
void writeU64(u8* p, u64 v) { memcpy(p, &v, sizeof(v)); }

u32 hashPosition(const u8* p) {
  // Multiplicative hash of the next 4 bytes (Knuth's 2654435761).
  return (readU32(p) * 2654435761u) >> (32 - kHashLog);
}

u64 rotl64(u64 x, int r) { return (x << r) | (x >> (64 - r)); }

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

}  // namespace

size_t compressBound(size_t rawSize) {
  // One extra byte per 255 literals for length continuations, plus slack for
  // the final token and this library's frame bookkeeping.
  return rawSize + rawSize / 255 + 16;
}

u64 hash64(const void* data, size_t size) {
  constexpr u64 kC1 = 0x87c37b91114253d5ull;
  constexpr u64 kC2 = 0x4cf5ad432745937full;
  const u8* p = static_cast<const u8*>(data);
  u64 h = 0x9e3779b97f4a7c15ull ^ (static_cast<u64>(size) * kC2);
  size_t n = size;
  while (n >= sizeof(u64)) {
    u64 k = readU64(p);
    k *= kC1;
    k = rotl64(k, 31);
    k *= kC2;
    h ^= k;
    h = rotl64(h, 27) * 5 + 0x52dce729;
    p += sizeof(u64);
    n -= sizeof(u64);
  }
  u64 tail = 0;
  for (size_t i = 0; i < n; ++i) {
    tail |= static_cast<u64>(p[i]) << (8 * i);
  }
  h ^= rotl64(tail * kC1, 31) * kC2;
  h ^= h >> 33;
  h *= 0xff51afd7ed558ccdull;
  h ^= h >> 33;
  h *= 0xc4ceb9fe1a85ec53ull;
  h ^= h >> 33;
  return h;
}

size_t compressBlock(const u8* src, size_t srcSize, u8* dst, size_t dstCapacity) {
  if (dstCapacity < compressBound(srcSize)) {
    return kError;
  }

  const u8* ip = src;
  const u8* const iend = src + srcSize;
  u8* op = dst;
  const u8* anchor = src;  // first literal not yet emitted

  auto emitSequence = [&](size_t litLen, size_t mLen, size_t offset) {
    u8* token = op++;
    // Literal length.
    if (litLen >= 15) {
      *token = 15 << 4;
      size_t rest = litLen - 15;
      while (rest >= 255) {
        *op++ = 255;
        rest -= 255;
      }
      *op++ = static_cast<u8>(rest);
    } else {
      *token = static_cast<u8>(litLen << 4);
    }
    memcpy(op, anchor, litLen);
    op += litLen;
    if (offset == 0) {
      return;  // final literal run, no match part
    }
    writeU16(op, static_cast<u16>(offset));
    op += 2;
    // Match length, biased by kMinMatch.
    size_t code = mLen - kMinMatch;
    if (code >= 15) {
      *token |= 15;
      code -= 15;
      while (code >= 255) {
        *op++ = 255;
        code -= 255;
      }
      *op++ = static_cast<u8>(code);
    } else {
      *token |= static_cast<u8>(code);
    }
  };

  if (srcSize >= kMfLimit + 1) {
    const u8* const mfLimit = iend - kMfLimit;
    const u8* const matchLimit = iend - kLastLiterals;
    // Positions are stored +1 so that 0 means "empty".
    U32Vector table(size_t(1) << kHashLog, 0);

    ++ip;  // position 0 has no previous occurrence to match
    while (ip < mfLimit) {
      // Find a match, accelerating the scan over incompressible regions.
      const u8* match;
      size_t searchCount = size_t(1) << kSkipTrigger;
      const u8* candidate = ip;
      for (;;) {
        u32 h = hashPosition(candidate);
        u32 prev = table[h];
        table[h] = static_cast<u32>(candidate - src) + 1;
        if (prev != 0) {
          match = src + prev - 1;
          if (static_cast<size_t>(candidate - match) <= kMaxOffset &&
              readU32(match) == readU32(candidate)) {
            ip = candidate;
            break;
          }
        }
        candidate += (searchCount++) >> kSkipTrigger;
        if (candidate >= mfLimit) {
          match = nullptr;
          break;
        }
      }
      if (match == nullptr) {
        break;
      }

      // Extend the match backwards over pending literals.
      while (ip > anchor && match > src && ip[-1] == match[-1]) {
        --ip;
        --match;
      }
      size_t mLen = kMinMatch + matchLength(ip + kMinMatch, match + kMinMatch, matchLimit);
      emitSequence(static_cast<size_t>(ip - anchor), mLen,
                   static_cast<size_t>(ip - match));
      ip += mLen;
      anchor = ip;
      if (ip >= mfLimit) {
        break;
      }
      // Seed the table with an intermediate position of the match just copied
      // so that runs and repeated structures keep chaining.
      table[hashPosition(ip - 2)] = static_cast<u32>(ip - 2 - src) + 1;
    }
  }

  // Trailing literals.
  emitSequence(static_cast<size_t>(iend - anchor), 0, 0);
  return static_cast<size_t>(op - dst);
}

size_t decompressBlock(const u8* src, size_t srcSize, u8* dst, size_t dstCapacity) {
  const u8* ip = src;
  const u8* const iend = src + srcSize;
  u8* op = dst;
  u8* const oend = dst + dstCapacity;

  // Reads a 255-continuation extension of a saturated nibble.
  auto readLengthExtension = [&](size_t& len) -> bool {
    u8 byte;
    do {
      if (ip >= iend) {
        return false;
      }
      byte = *ip++;
      len += byte;
    } while (byte == 255);
    return true;
  };

  if (srcSize == 0) {
    return kError;
  }
  for (;;) {
    if (ip >= iend) {
      return kError;
    }
    const u8 token = *ip++;

    // Literals.
    size_t litLen = token >> 4;
    if (litLen < 15 && litLen + 16 <= static_cast<size_t>(iend - ip) &&
        litLen + 16 <= static_cast<size_t>(oend - op)) {
      // Fast path: a short literal run with slack on both sides — one
      // unconditional 16-byte copy regardless of the exact length.
      memcpy(op, ip, 16);
    } else {
      if (litLen == 15 && !readLengthExtension(litLen)) {
        return kError;
      }
      if (litLen > static_cast<size_t>(iend - ip) ||
          litLen > static_cast<size_t>(oend - op)) {
        return kError;
      }
      memcpy(op, ip, litLen);
    }
    ip += litLen;
    op += litLen;
    if (ip == iend) {
      // A block always ends with a literals-only sequence.
      return static_cast<size_t>(op - dst);
    }

    // Match.
    if (iend - ip < 2) {
      return kError;
    }
    size_t offset = ip[0] | (static_cast<size_t>(ip[1]) << 8);
    ip += 2;
    if (offset == 0 || offset > static_cast<size_t>(op - dst)) {
      return kError;
    }
    size_t mLen = (token & 0xf);
    if (mLen == 15 && !readLengthExtension(mLen)) {
      return kError;
    }
    mLen += kMinMatch;
    if (mLen > static_cast<size_t>(oend - op)) {
      return kError;
    }
    const u8* match = op - offset;
    if (offset >= sizeof(u64)) {
      // Non-overlapping (per 8-byte chunk): copy in word-sized steps. Each
      // chunk's source lies at least 8 bytes behind its destination, so
      // already-copied bytes are always valid to read.
      u8* out = op;
      op += mLen;
      if (op + sizeof(u64) <= oend) {
        // Wild copy: overshoot to the next 8-byte boundary within the slack.
        do {
          memcpy(out, match, sizeof(u64));
          out += sizeof(u64);
          match += sizeof(u64);
        } while (out < op);
      } else {
        while (mLen >= sizeof(u64)) {
          memcpy(out, match, sizeof(u64));
          out += sizeof(u64);
          match += sizeof(u64);
          mLen -= sizeof(u64);
        }
        memcpy(out, match, mLen);
      }
    } else {
      // Overlapping copy (e.g. RLE with offset 1) must go byte by byte.
      while (mLen--) {
        *op++ = *match++;
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Frame layer.
//
//   u32 magic | u8 codec (Level)
//   repeated blocks: u32 rawSize (1..kFrameBlockSize) | u32 compressedSize
//                    (0 = stored raw) | payload
//   u32 rawSize == 0 terminator
//   u64 checksum, chained per block: chk = hash64(block) ^ rotl64(chk, 1)

namespace {

constexpr size_t kFrameHeaderSize = 4 + 1;

u64 chainChecksum(u64 chk, const u8* raw, size_t rawSize) {
  return hash64(raw, rawSize) ^ rotl64(chk, 1);
}

// Compress one frame block with the requested codec. May return kError
// (Rc gives up on incompressible input rather than overflow scratch), which
// compares >= rawSize, so callers store the block raw in that case too.
size_t frameCompressBlock(Level level, const u8* src, size_t srcSize, u8* dst,
                          size_t dstCapacity) {
  return level == Level::kFast ? compressBlock(src, srcSize, dst, dstCapacity)
                               : Rc::compressBlock(src, srcSize, dst, dstCapacity);
}

size_t frameDecompressBlock(Level level, const u8* src, size_t srcSize, u8* dst,
                            size_t dstCapacity) {
  return level == Level::kFast ? decompressBlock(src, srcSize, dst, dstCapacity)
                               : Rc::decompressBlock(src, srcSize, dst, dstCapacity);
}

bool validLevel(u8 codec) {
  return codec == static_cast<u8>(Level::kFast) || codec == static_cast<u8>(Level::kHigh);
}

struct FileCloser {
  void operator()(FILE* f) const {
    if (f) {
      fclose(f);
    }
  }
};
using FilePtr = unique_ptr<FILE, FileCloser>;

}  // namespace

vector<u8> compress(const void* data, size_t size, Level level) {
  const u8* src = static_cast<const u8*>(data);
  vector<u8> out(kFrameHeaderSize);
  writeU32(out.data(), kMagic);
  out[4] = static_cast<u8>(level);
  const size_t blockBound = level == Level::kFast
                                ? compressBound(min(size, kFrameBlockSize))
                                : Rc::compressBound(min(size, kFrameBlockSize));
  vector<u8> scratch(blockBound);
  u64 chk = 0;
  for (size_t pos = 0; pos < size; pos += kFrameBlockSize) {
    const size_t rawSize = min(kFrameBlockSize, size - pos);
    const size_t compSize =
        frameCompressBlock(level, src + pos, rawSize, scratch.data(), scratch.size());
    const bool stored = compSize >= rawSize;  // also catches kError
    const size_t payloadSize = stored ? rawSize : compSize;
    const size_t base = out.size();
    out.resize(base + 8 + payloadSize);
    writeU32(out.data() + base, static_cast<u32>(rawSize));
    writeU32(out.data() + base + 4, stored ? 0 : static_cast<u32>(compSize));
    memcpy(out.data() + base + 8, stored ? src + pos : scratch.data(), payloadSize);
    chk = chainChecksum(chk, src + pos, rawSize);
  }
  const size_t base = out.size();
  out.resize(base + 4 + 8);
  writeU32(out.data() + base, 0);
  writeU64(out.data() + base + 4, chk);
  return out;
}

vector<u8> compress(const vector<u8>& data, Level level) {
  return compress(data.data(), data.size(), level);
}

vector<u8> compress(const string& data, Level level) {
  return compress(data.data(), data.size(), level);
}

bool decompress(const u8* data, size_t size, vector<u8>& out) {
  out.clear();
  size_t pos = 0;
  auto remaining = [&] { return size - pos; };
  if (remaining() < kFrameHeaderSize || readU32(data) != kMagic || !validLevel(data[4])) {
    return false;
  }
  const Level level = static_cast<Level>(data[4]);
  pos = kFrameHeaderSize;
  u64 chk = 0;
  for (;;) {
    if (remaining() < 4) {
      return false;
    }
    const size_t rawSize = readU32(data + pos);
    pos += 4;
    if (rawSize == 0) {
      break;
    }
    if (rawSize > kFrameBlockSize || remaining() < 4) {
      return false;
    }
    const size_t compSize = readU32(data + pos);
    pos += 4;
    const size_t payloadSize = compSize == 0 ? rawSize : compSize;
    if (payloadSize > remaining()) {
      return false;
    }
    const size_t base = out.size();
    out.resize(base + rawSize);
    if (compSize == 0) {
      memcpy(out.data() + base, data + pos, rawSize);
    } else if (frameDecompressBlock(level, data + pos, compSize, out.data() + base,
                                    rawSize) != rawSize) {
      return false;
    }
    pos += payloadSize;
    chk = chainChecksum(chk, out.data() + base, rawSize);
  }
  return remaining() == 8 && readU64(data + pos) == chk;
}

vector<u8> decompress(const vector<u8>& data) {
  vector<u8> out;
  if (!decompress(data.data(), data.size(), out)) {
    out.clear();
  }
  return out;
}

bool compressFile(const string& inputPath, const string& outputPath, Level level) {
  FilePtr in(fopen(inputPath.c_str(), "rb"));
  FilePtr fout(fopen(outputPath.c_str(), "wb"));
  if (!in || !fout) {
    return false;
  }
  u8 header[kFrameHeaderSize];
  writeU32(header, kMagic);
  header[4] = static_cast<u8>(level);
  if (fwrite(header, 1, sizeof(header), fout.get()) != sizeof(header)) {
    return false;
  }
  vector<u8> raw(kFrameBlockSize);
  vector<u8> scratch(max(compressBound(kFrameBlockSize), Rc::compressBound(kFrameBlockSize)));
  u64 chk = 0;
  for (;;) {
    const size_t rawSize = fread(raw.data(), 1, raw.size(), in.get());
    if (rawSize == 0) {
      break;
    }
    const size_t compSize =
        frameCompressBlock(level, raw.data(), rawSize, scratch.data(), scratch.size());
    const bool stored = compSize >= rawSize;  // also catches kError
    u8 blockHeader[8];
    writeU32(blockHeader, static_cast<u32>(rawSize));
    writeU32(blockHeader + 4, stored ? 0 : static_cast<u32>(compSize));
    const u8* payload = stored ? raw.data() : scratch.data();
    const size_t payloadSize = stored ? rawSize : compSize;
    if (fwrite(blockHeader, 1, sizeof(blockHeader), fout.get()) != sizeof(blockHeader) ||
        fwrite(payload, 1, payloadSize, fout.get()) != payloadSize) {
      return false;
    }
    chk = chainChecksum(chk, raw.data(), rawSize);
  }
  if (ferror(in.get())) {
    return false;
  }
  u8 trailer[12];
  writeU32(trailer, 0);
  writeU64(trailer + 4, chk);
  return fwrite(trailer, 1, sizeof(trailer), fout.get()) == sizeof(trailer) &&
         fflush(fout.get()) == 0;
}

bool decompressFile(const string& inputPath, const string& outputPath) {
  FilePtr in(fopen(inputPath.c_str(), "rb"));
  FilePtr fout(fopen(outputPath.c_str(), "wb"));
  if (!in || !fout) {
    return false;
  }
  u8 header[kFrameHeaderSize];
  if (fread(header, 1, sizeof(header), in.get()) != sizeof(header) ||
      readU32(header) != kMagic || !validLevel(header[4])) {
    return false;
  }
  const Level level = static_cast<Level>(header[4]);
  vector<u8> payload(compressBound(kFrameBlockSize));
  vector<u8> raw(kFrameBlockSize);
  u64 chk = 0;
  for (;;) {
    u8 blockHeader[8];
    if (fread(blockHeader, 1, 4, in.get()) != 4) {
      return false;
    }
    const size_t rawSize = readU32(blockHeader);
    if (rawSize == 0) {
      break;
    }
    if (rawSize > kFrameBlockSize ||
        fread(blockHeader + 4, 1, 4, in.get()) != 4) {
      return false;
    }
    const size_t compSize = readU32(blockHeader + 4);
    if (compSize > payload.size()) {
      return false;
    }
    const size_t payloadSize = compSize == 0 ? rawSize : compSize;
    if (fread(payload.data(), 1, payloadSize, in.get()) != payloadSize) {
      return false;
    }
    const u8* rawOut = payload.data();
    if (compSize != 0) {
      if (frameDecompressBlock(level, payload.data(), compSize, raw.data(), rawSize) !=
          rawSize) {
        return false;
      }
      rawOut = raw.data();
    }
    if (fwrite(rawOut, 1, rawSize, fout.get()) != rawSize) {
      return false;
    }
    chk = chainChecksum(chk, rawOut, rawSize);
  }
  u8 trailer[8];
  return fread(trailer, 1, sizeof(trailer), in.get()) == sizeof(trailer) &&
         readU64(trailer) == chk && fflush(fout.get()) == 0;
}

}  // namespace Lz
