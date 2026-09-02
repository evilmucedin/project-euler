#include "gtest/gtest.h"

#include "lib/compression/lz.h"
#include "lib/compression/rc.h"

#include <random>

namespace {

vector<u8> toBytes(const string& s) {
  return vector<u8>(s.begin(), s.end());
}

vector<u8> randomBytes(size_t n, u32 seed) {
  mt19937 rng(seed);
  vector<u8> data(n);
  for (auto& b : data) {
    b = static_cast<u8>(rng());
  }
  return data;
}

vector<u8> compressibleText(size_t approxSize) {
  static const StringVector words = {"the", "quick", "brown", "fox", "jumps",
                                     "over", "lazy", "dog", "compression"};
  string s;
  mt19937 rng(12345);
  while (s.size() < approxSize) {
    s += words[rng() % words.size()];
    s += ' ';
  }
  return toBytes(s);
}

// Round-trips through the block API; returns the compressed size.
size_t BlockRoundTrip(const vector<u8>& data) {
  vector<u8> dst(Rc::compressBound(data.size()));
  const size_t compSize = Rc::compressBlock(data.data(), data.size(), dst.data(), dst.size());
  EXPECT_NE(compSize, Rc::kError);
  vector<u8> got(data.size());
  EXPECT_EQ(Rc::decompressBlock(dst.data(), compSize, got.data(), got.size()), data.size());
  EXPECT_EQ(got, data);
  return compSize;
}

}  // namespace

TEST(RcBlockTest, RoundTripVariedInputs) {
  BlockRoundTrip({});
  BlockRoundTrip({0});
  BlockRoundTrip(toBytes("a"));
  BlockRoundTrip(toBytes("ab"));
  BlockRoundTrip(toBytes("hello, world"));
  BlockRoundTrip(vector<u8>(100000, u8('x')));
  for (size_t n : {12u, 13u, 100u, 4096u, 65536u, 1u << 20}) {
    vector<u8> data = compressibleText(n);
    data.resize(n);
    BlockRoundTrip(data);
  }
  BlockRoundTrip(randomBytes(1 << 18, 7));
}

TEST(RcBlockTest, RepeatedStructureUsesRepDistances) {
  // Fixed-stride records: rep0 matches should make this shrink drastically.
  vector<u8> data;
  for (u32 i = 0; i < 20000; ++i) {
    data.push_back(static_cast<u8>(i & 0xf));
    data.push_back('=');
    data.push_back('#');
    data.push_back(';');
  }
  const size_t compSize = BlockRoundTrip(data);
  EXPECT_LT(compSize, data.size() / 20);
}

TEST(RcBlockTest, BeatsFastCodecOnText) {
  const vector<u8> data = compressibleText(1 << 20);
  vector<u8> rcDst(Rc::compressBound(data.size()));
  const size_t rcSize =
      Rc::compressBlock(data.data(), data.size(), rcDst.data(), rcDst.size());
  ASSERT_NE(rcSize, Rc::kError);
  vector<u8> lzDst(Lz::compressBound(data.size()));
  const size_t lzSize =
      Lz::compressBlock(data.data(), data.size(), lzDst.data(), lzDst.size());
  ASSERT_NE(lzSize, Lz::kError);
  EXPECT_LT(rcSize, lzSize);
}

TEST(RcBlockTest, TightBufferReturnsError) {
  const vector<u8> data = randomBytes(4096, 9);
  vector<u8> dst(data.size() / 2);
  EXPECT_EQ(Rc::compressBlock(data.data(), data.size(), dst.data(), dst.size()), Rc::kError);
}

TEST(RcBlockTest, DecompressRejectsCorruptInput) {
  const vector<u8> data = compressibleText(65536);
  vector<u8> dst(Rc::compressBound(data.size()));
  const size_t compSize = Rc::compressBlock(data.data(), data.size(), dst.data(), dst.size());
  ASSERT_NE(compSize, Rc::kError);
  vector<u8> got(data.size());
  // Heavy truncation must fail cleanly (the checksum in the frame layer
  // catches milder corruption that still decodes to full length).
  EXPECT_EQ(Rc::decompressBlock(dst.data(), compSize / 4, got.data(), got.size()),
            Rc::kError);
  EXPECT_EQ(Rc::decompressBlock(dst.data(), 2, got.data(), got.size()), Rc::kError);
}

TEST(RcFrameTest, HighLevelRoundTrips) {
  for (const vector<u8>& data :
       {vector<u8>{}, toBytes("hello"), compressibleText(3 << 20),
        randomBytes((1 << 20) + 1, 3)}) {
    const vector<u8> packed = Lz::compress(data, Lz::Level::kHigh);
    vector<u8> got;
    ASSERT_TRUE(Lz::decompress(packed.data(), packed.size(), got));
    EXPECT_EQ(got, data);
  }
}

TEST(RcFrameTest, HighLevelBeatsFastLevel) {
  const vector<u8> data = compressibleText(2 << 20);
  EXPECT_LT(Lz::compress(data, Lz::Level::kHigh).size(),
            Lz::compress(data, Lz::Level::kFast).size());
}

TEST(RcFrameTest, DetectsCorruption) {
  const vector<u8> data = compressibleText(100000);
  vector<u8> packed = Lz::compress(data, Lz::Level::kHigh);
  vector<u8> out;
  EXPECT_FALSE(Lz::decompress(packed.data(), packed.size() - 1, out));
  packed[packed.size() / 2] ^= 0xff;
  EXPECT_FALSE(Lz::decompress(packed.data(), packed.size(), out));
  // An unknown codec byte is rejected.
  vector<u8> bad = Lz::compress(data, Lz::Level::kFast);
  bad[4] = 0x7f;
  EXPECT_FALSE(Lz::decompress(bad.data(), bad.size(), out));
}
