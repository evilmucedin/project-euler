#include "gtest/gtest.h"

#include "lib/compression/lz.h"

#include <cstdio>
#include <random>

namespace {

vector<u8> toBytes(const string& s) {
  return vector<u8>(s.begin(), s.end());
}

void ExpectRoundTrip(const vector<u8>& data) {
  const vector<u8> packed = Lz::compress(data);
  vector<u8> got;
  ASSERT_TRUE(Lz::decompress(packed.data(), packed.size(), got));
  EXPECT_EQ(got, data);
}

vector<u8> randomBytes(size_t n, u32 seed, u32 alphabet = 256) {
  mt19937 rng(seed);
  vector<u8> data(n);
  for (auto& b : data) {
    b = static_cast<u8>(rng() % alphabet);
  }
  return data;
}

// Repetitive, highly compressible text.
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

class TempFile {
 public:
  explicit TempFile(const string& tag) {
    path_ = string(::testing::TempDir()) + "lz_test_" + tag;
  }
  ~TempFile() { remove(path_.c_str()); }
  const string& path() const { return path_; }

 private:
  string path_;
};

}  // namespace

TEST(LzBlockTest, EmptyRoundTrip) {
  vector<u8> dst(Lz::compressBound(0));
  const size_t compSize = Lz::compressBlock(nullptr, 0, dst.data(), dst.size());
  ASSERT_NE(compSize, Lz::kError);
  EXPECT_EQ(Lz::decompressBlock(dst.data(), compSize, nullptr, 0), 0u);
}

TEST(LzBlockTest, RoundTripVariedSizes) {
  for (size_t n : {1u, 2u, 12u, 13u, 64u, 100u, 65536u, 1u << 20}) {
    vector<u8> data = compressibleText(n);
    data.resize(n);
    vector<u8> dst(Lz::compressBound(n));
    const size_t compSize = Lz::compressBlock(data.data(), n, dst.data(), dst.size());
    ASSERT_NE(compSize, Lz::kError);
    vector<u8> got(n);
    ASSERT_EQ(Lz::decompressBlock(dst.data(), compSize, got.data(), n), n) << n;
    EXPECT_EQ(got, data) << n;
  }
}

TEST(LzBlockTest, CompressesRepetitiveData) {
  const vector<u8> data(100000, u8('a'));
  vector<u8> dst(Lz::compressBound(data.size()));
  const size_t compSize = Lz::compressBlock(data.data(), data.size(), dst.data(), dst.size());
  ASSERT_NE(compSize, Lz::kError);
  EXPECT_LT(compSize, data.size() / 100);
}

TEST(LzBlockTest, TextCompressionRatio) {
  const vector<u8> data = compressibleText(1 << 20);
  vector<u8> dst(Lz::compressBound(data.size()));
  const size_t compSize = Lz::compressBlock(data.data(), data.size(), dst.data(), dst.size());
  ASSERT_NE(compSize, Lz::kError);
  // Repeating dictionary of 9 short words should shrink by well over 2x.
  EXPECT_LT(compSize, data.size() / 2);
}

TEST(LzBlockTest, IncompressibleStaysWithinBound) {
  const vector<u8> data = randomBytes(1 << 18, 7);
  vector<u8> dst(Lz::compressBound(data.size()));
  const size_t compSize = Lz::compressBlock(data.data(), data.size(), dst.data(), dst.size());
  ASSERT_NE(compSize, Lz::kError);
  EXPECT_LE(compSize, Lz::compressBound(data.size()));
  vector<u8> got(data.size());
  ASSERT_EQ(Lz::decompressBlock(dst.data(), compSize, got.data(), got.size()), data.size());
  EXPECT_EQ(got, data);
}

TEST(LzBlockTest, DecompressRejectsCorruptInput) {
  const vector<u8> data = compressibleText(4096);
  vector<u8> dst(Lz::compressBound(data.size()));
  const size_t compSize = Lz::compressBlock(data.data(), data.size(), dst.data(), dst.size());
  ASSERT_NE(compSize, Lz::kError);
  vector<u8> got(data.size());
  // Truncations must never crash and must fail or produce a short result.
  for (size_t cut = 1; cut < compSize; cut += 7) {
    const size_t n = Lz::decompressBlock(dst.data(), compSize - cut, got.data(), got.size());
    EXPECT_TRUE(n == Lz::kError || n <= data.size());
  }
  // Undersized output buffer is an error, not an overflow.
  EXPECT_EQ(Lz::decompressBlock(dst.data(), compSize, got.data(), data.size() / 2), Lz::kError);
}

TEST(LzFrameTest, MemoryRoundTrips) {
  ExpectRoundTrip({});
  ExpectRoundTrip({42});
  ExpectRoundTrip(toBytes("hello, world"));
  ExpectRoundTrip(compressibleText(3 << 20));       // several frame blocks
  ExpectRoundTrip(randomBytes(2 << 20, 3));          // stored (incompressible) blocks
  ExpectRoundTrip(vector<u8>(1 << 20, u8(0)));       // exactly one frame block
  ExpectRoundTrip(randomBytes((1 << 20) + 1, 4));    // one byte over a block
}

TEST(LzFrameTest, FrameOverheadIsSmall) {
  const vector<u8> data = randomBytes(1 << 20, 5);
  const vector<u8> packed = Lz::compress(data);
  EXPECT_LT(packed.size(), data.size() + 64);
}

TEST(LzFrameTest, DetectsCorruption) {
  const vector<u8> data = compressibleText(100000);
  vector<u8> packed = Lz::compress(data);
  vector<u8> out;
  EXPECT_FALSE(Lz::decompress(packed.data(), packed.size() - 1, out));
  packed[packed.size() / 2] ^= 0xff;
  EXPECT_FALSE(Lz::decompress(packed.data(), packed.size(), out));
  EXPECT_FALSE(Lz::decompress(nullptr, 0, out));
}

TEST(LzFrameTest, ConvenienceDecompressReturnsEmptyOnError) {
  EXPECT_TRUE(Lz::decompress(vector<u8>{1, 2, 3}).empty());
  const vector<u8> data = toBytes("some payload");
  EXPECT_EQ(Lz::decompress(Lz::compress(data)), data);
}

TEST(LzFileTest, FileRoundTrip) {
  const vector<u8> data = compressibleText(5 << 20);
  TempFile raw("raw"), packed("packed"), restored("restored");
  {
    FILE* f = fopen(raw.path().c_str(), "wb");
    ASSERT_NE(f, nullptr);
    ASSERT_EQ(fwrite(data.data(), 1, data.size(), f), data.size());
    fclose(f);
  }
  ASSERT_TRUE(Lz::compressFile(raw.path(), packed.path()));
  ASSERT_TRUE(Lz::decompressFile(packed.path(), restored.path()));
  FILE* f = fopen(restored.path().c_str(), "rb");
  ASSERT_NE(f, nullptr);
  vector<u8> got(data.size() + 1);
  const size_t n = fread(got.data(), 1, got.size(), f);
  fclose(f);
  got.resize(n);
  EXPECT_EQ(got, data);
}

TEST(LzFileTest, HighLevelFileRoundTrip) {
  const vector<u8> data = compressibleText(2 << 20);
  TempFile raw("hi_raw"), packed("hi_packed"), restored("hi_restored");
  {
    FILE* f = fopen(raw.path().c_str(), "wb");
    ASSERT_NE(f, nullptr);
    ASSERT_EQ(fwrite(data.data(), 1, data.size(), f), data.size());
    fclose(f);
  }
  ASSERT_TRUE(Lz::compressFile(raw.path(), packed.path(), Lz::Level::kHigh));
  ASSERT_TRUE(Lz::decompressFile(packed.path(), restored.path()));
  FILE* f = fopen(restored.path().c_str(), "rb");
  ASSERT_NE(f, nullptr);
  vector<u8> got(data.size() + 1);
  got.resize(fread(got.data(), 1, got.size(), f));
  fclose(f);
  EXPECT_EQ(got, data);
}

TEST(LzFileTest, EmptyFileRoundTrip) {
  TempFile raw("empty_raw"), packed("empty_packed"), restored("empty_restored");
  fclose(fopen(raw.path().c_str(), "wb"));
  ASSERT_TRUE(Lz::compressFile(raw.path(), packed.path()));
  ASSERT_TRUE(Lz::decompressFile(packed.path(), restored.path()));
  FILE* f = fopen(restored.path().c_str(), "rb");
  ASSERT_NE(f, nullptr);
  u8 byte;
  EXPECT_EQ(fread(&byte, 1, 1, f), 0u);
  fclose(f);
}

TEST(LzFileTest, MissingInputFails) {
  TempFile out("missing_out");
  EXPECT_FALSE(Lz::compressFile("/nonexistent/path/in", out.path()));
  EXPECT_FALSE(Lz::decompressFile("/nonexistent/path/in", out.path()));
}

TEST(LzFileTest, CorruptFileFails) {
  const vector<u8> data = compressibleText(200000);
  TempFile raw("c_raw"), packed("c_packed"), restored("c_restored");
  {
    FILE* f = fopen(raw.path().c_str(), "wb");
    ASSERT_NE(f, nullptr);
    ASSERT_EQ(fwrite(data.data(), 1, data.size(), f), data.size());
    fclose(f);
  }
  ASSERT_TRUE(Lz::compressFile(raw.path(), packed.path()));
  // Flip one byte in the middle of the compressed file.
  FILE* f = fopen(packed.path().c_str(), "r+b");
  ASSERT_NE(f, nullptr);
  fseek(f, 0, SEEK_END);
  const long size = ftell(f);
  fseek(f, size / 2, SEEK_SET);
  int c = fgetc(f);
  fseek(f, size / 2, SEEK_SET);
  fputc(c ^ 0xff, f);
  fclose(f);
  EXPECT_FALSE(Lz::decompressFile(packed.path(), restored.path()));
}

TEST(LzHashTest, Hash64Basics) {
  EXPECT_NE(Lz::hash64("a", 1), Lz::hash64("b", 1));
  EXPECT_NE(Lz::hash64("", 0), Lz::hash64("a", 1));
  const string s = "the same content hashes the same";
  EXPECT_EQ(Lz::hash64(s.data(), s.size()), Lz::hash64(s.data(), s.size()));
}
