#include "gtest/gtest.h"

#include "lib/compression/pfordelta.h"

#include <vector>

namespace {

using PForDelta::compress;
using PForDelta::decompress;
using PForDelta::kBlockSize;

void ExpectRoundTrip(const U32Vector& values) {
  const vector<u8> packed = compress(values);
  const U32Vector got = decompress(packed);
  EXPECT_EQ(got, values);
}

}  // namespace

TEST(PForDeltaTest, EmptyRoundTrip) {
  ExpectRoundTrip({});
}

TEST(PForDeltaTest, SingleValue) {
  ExpectRoundTrip({42});
  ExpectRoundTrip({0});
  ExpectRoundTrip({0xffffffffu});
}

TEST(PForDeltaTest, SortedSmallDeltas) {
  U32Vector v;
  for (u32 i = 0; i < 1000; ++i) v.push_back(i * 3);
  ExpectRoundTrip(v);
}

TEST(PForDeltaTest, ExactBlockSize) {
  U32Vector v;
  v.reserve(kBlockSize);
  for (u32 i = 0; i < kBlockSize; ++i) v.push_back(i);
  ExpectRoundTrip(v);
}

TEST(PForDeltaTest, MultipleBlocks) {
  U32Vector v;
  for (u32 i = 0; i < kBlockSize * 3 + 17; ++i) v.push_back(i / 2);
  ExpectRoundTrip(v);
}

TEST(PForDeltaTest, ConstantBlock) {
  ExpectRoundTrip(U32Vector(500, 999u));
}

TEST(PForDeltaTest, ExceptionsInBlock) {
  // One large outlier forces exceptions while rest are small deltas.
  U32Vector v;
  for (u32 i = 0; i < 64; ++i) v.push_back(i);
  v.push_back(0x12345678u);
  for (u32 i = 0; i < 63; ++i) v.push_back(100 + i);
  ExpectRoundTrip(v);
}

TEST(PForDeltaTest, DecompressTruncatedReturnsEmpty) {
  U32Vector original = {1, 2, 3, 4, 5};
  vector<u8> blob = compress(original);
  ASSERT_GT(blob.size(), 4u);
  blob.resize(blob.size() - 3);
  EXPECT_TRUE(decompress(blob).empty());
}

TEST(PForDeltaTest, DecompressGarbageReturnsEmpty) {
  vector<u8> bad = {0, 0, 0, 0};
  EXPECT_TRUE(decompress(bad).empty());
}

TEST(PForDeltaTest, DecompressCorruptCountReturnsEmpty) {
  U32Vector original = {10, 20};
  vector<u8> blob = compress(original);
  // total_count at offset 0 — set impossibly large so reads fail.
  blob[0] = 0xff;
  blob[1] = 0xff;
  blob[2] = 0xff;
  blob[3] = 0xff;
  EXPECT_TRUE(decompress(blob).empty());
}
