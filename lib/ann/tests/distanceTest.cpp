#include "lib/ann/distance.h"

#include "gtest/gtest.h"

#include <cmath>
#include <random>
#include <vector>

namespace {

double l2SqReference(const std::vector<float>& a, const std::vector<float>& b) {
    double s = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        const double d = static_cast<double>(a[i]) - static_cast<double>(b[i]);
        s += d * d;
    }
    return s;
}

double dotReference(const std::vector<float>& a, const std::vector<float>& b) {
    double s = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        s += static_cast<double>(a[i]) * static_cast<double>(b[i]);
    }
    return s;
}

std::vector<float> randomVector(std::mt19937& rng, std::size_t n) {
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);
    std::vector<float> v(n);
    for (float& x : v) {
        x = dist(rng);
    }
    return v;
}

}  // namespace

TEST(Distance, MatchesReferenceForEveryLength) {
    std::mt19937 rng(7);
    for (std::size_t n = 0; n <= 100; ++n) {
        const std::vector<float> a = randomVector(rng, n);
        const std::vector<float> b = randomVector(rng, n);
        const double l2 = l2SqReference(a, b);
        const double dp = dotReference(a, b);
        const double tol = 1e-4 * (1.0 + std::fabs(l2) + std::fabs(dp));
        EXPECT_NEAR(ann::l2Sq(a.data(), b.data(), n), l2, tol) << "n=" << n;
        EXPECT_NEAR(ann::dot(a.data(), b.data(), n), dp, tol) << "n=" << n;
        EXPECT_NEAR(ann::detail::l2SqScalar(a.data(), b.data(), n), l2, tol) << "n=" << n;
        EXPECT_NEAR(ann::detail::dotScalar(a.data(), b.data(), n), dp, tol) << "n=" << n;
    }
}

TEST(Distance, SimdAndScalarAgree) {
    std::mt19937 rng(11);
    for (std::size_t n : {1u, 3u, 7u, 8u, 15u, 16u, 31u, 32u, 33u, 64u, 100u, 257u}) {
        const std::vector<float> a = randomVector(rng, n);
        const std::vector<float> b = randomVector(rng, n);
        const float tol = 1e-4f * static_cast<float>(n);
        EXPECT_NEAR(ann::l2Sq(a.data(), b.data(), n), ann::detail::l2SqScalar(a.data(), b.data(), n), tol);
        EXPECT_NEAR(ann::dot(a.data(), b.data(), n), ann::detail::dotScalar(a.data(), b.data(), n), tol);
    }
}

TEST(Distance, UnalignedPointers) {
    std::mt19937 rng(13);
    const std::vector<float> a = randomVector(rng, 70);
    const std::vector<float> b = randomVector(rng, 70);
    for (std::size_t off = 0; off < 5; ++off) {
        const std::size_t n = 64 - off;
        const std::vector<float> sa(a.begin() + off, a.begin() + off + n);
        const std::vector<float> sb(b.begin() + off + 1, b.begin() + off + 1 + n);
        EXPECT_NEAR(ann::l2Sq(a.data() + off, b.data() + off + 1, n), l2SqReference(sa, sb), 1e-3);
    }
}

TEST(Distance, ZeroAndSelf) {
    const std::vector<float> a = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    EXPECT_FLOAT_EQ(ann::l2Sq(a.data(), a.data(), a.size()), 0.0f);
    EXPECT_FLOAT_EQ(ann::squaredNorm(a.data(), a.size()), 55.0f);
    EXPECT_FLOAT_EQ(ann::norm(a.data(), a.size()), std::sqrt(55.0f));
    EXPECT_FLOAT_EQ(ann::dot(a.data(), a.data(), 0), 0.0f);
}

TEST(Distance, NormalizeMakesUnitLength) {
    std::vector<float> v = {3.0f, 0.0f, 4.0f};
    ann::normalize(v.data(), v.size());
    EXPECT_NEAR(ann::norm(v.data(), v.size()), 1.0f, 1e-6f);
    EXPECT_NEAR(v[0], 0.6f, 1e-6f);
    EXPECT_NEAR(v[2], 0.8f, 1e-6f);

    std::vector<float> zero(4, 0.0f);
    ann::normalize(zero.data(), zero.size());
    for (float x : zero) {
        EXPECT_EQ(x, 0.0f);
    }
}

TEST(Distance, MetricPolicies) {
    const std::vector<float> a = {1.0f, 0.0f};
    const std::vector<float> b = {0.0f, 1.0f};
    EXPECT_FLOAT_EQ(ann::L2::distance(a.data(), b.data(), 2), 2.0f);
    EXPECT_FLOAT_EQ(ann::InnerProduct::distance(a.data(), b.data(), 2), 0.0f);
    EXPECT_FLOAT_EQ(ann::InnerProduct::distance(a.data(), a.data(), 2), -1.0f);
    EXPECT_FLOAT_EQ(ann::Cosine::distance(a.data(), b.data(), 2), 1.0f);
    EXPECT_FLOAT_EQ(ann::Cosine::distance(a.data(), a.data(), 2), 0.0f);
    EXPECT_FALSE(ann::L2::kNormalizeOnInsert);
    EXPECT_TRUE(ann::Cosine::kNormalizeOnInsert);
}
