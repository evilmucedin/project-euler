#pragma once

// Distance kernels and metric policies.
//
// The kernels run on explicit AVX2/FMA or NEON intrinsics when the compiler
// targets them (-march=native in the Buck2 toolchain) and fall back to a
// multi-accumulator scalar loop that auto-vectorizes well. Define ANN_NO_SIMD
// to force the scalar path. All kernels accept any length and any alignment.

#include "lib/ann/common.h"

#include <cmath>
#include <cstddef>

#if !defined(ANN_NO_SIMD)
#if defined(__AVX2__) && defined(__FMA__)
#include <immintrin.h>
#define ANN_HAVE_AVX2 1
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#define ANN_HAVE_NEON 1
#endif
#endif

namespace ann {
namespace detail {

// Reference kernels. Eight independent accumulators let the optimizer emit
// vector code without changing the result order between runs.
inline float l2SqScalar(const float* a, const float* b, std::size_t n) noexcept {
    float acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        for (std::size_t j = 0; j < 8; ++j) {
            const float d = a[i + j] - b[i + j];
            acc[j] += d * d;
        }
    }
    float sum = ((acc[0] + acc[1]) + (acc[2] + acc[3])) + ((acc[4] + acc[5]) + (acc[6] + acc[7]));
    for (; i < n; ++i) {
        const float d = a[i] - b[i];
        sum += d * d;
    }
    return sum;
}

inline float dotScalar(const float* a, const float* b, std::size_t n) noexcept {
    float acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        for (std::size_t j = 0; j < 8; ++j) {
            acc[j] += a[i + j] * b[i + j];
        }
    }
    float sum = ((acc[0] + acc[1]) + (acc[2] + acc[3])) + ((acc[4] + acc[5]) + (acc[6] + acc[7]));
    for (; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

#if defined(ANN_HAVE_AVX2)

ANN_ALWAYS_INLINE float horizontalSum(__m256 v) noexcept {
    __m128 lo = _mm256_castps256_ps128(v);
    const __m128 hi = _mm256_extractf128_ps(v, 1);
    lo = _mm_add_ps(lo, hi);
    __m128 shuf = _mm_movehdup_ps(lo);
    __m128 sums = _mm_add_ps(lo, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    return _mm_cvtss_f32(sums);
}

inline float l2SqSimd(const float* a, const float* b, std::size_t n) noexcept {
    __m256 s0 = _mm256_setzero_ps();
    __m256 s1 = _mm256_setzero_ps();
    __m256 s2 = _mm256_setzero_ps();
    __m256 s3 = _mm256_setzero_ps();
    std::size_t i = 0;
    for (; i + 32 <= n; i += 32) {
        const __m256 d0 = _mm256_sub_ps(_mm256_loadu_ps(a + i), _mm256_loadu_ps(b + i));
        const __m256 d1 = _mm256_sub_ps(_mm256_loadu_ps(a + i + 8), _mm256_loadu_ps(b + i + 8));
        const __m256 d2 = _mm256_sub_ps(_mm256_loadu_ps(a + i + 16), _mm256_loadu_ps(b + i + 16));
        const __m256 d3 = _mm256_sub_ps(_mm256_loadu_ps(a + i + 24), _mm256_loadu_ps(b + i + 24));
        s0 = _mm256_fmadd_ps(d0, d0, s0);
        s1 = _mm256_fmadd_ps(d1, d1, s1);
        s2 = _mm256_fmadd_ps(d2, d2, s2);
        s3 = _mm256_fmadd_ps(d3, d3, s3);
    }
    for (; i + 8 <= n; i += 8) {
        const __m256 d = _mm256_sub_ps(_mm256_loadu_ps(a + i), _mm256_loadu_ps(b + i));
        s0 = _mm256_fmadd_ps(d, d, s0);
    }
    float sum = horizontalSum(_mm256_add_ps(_mm256_add_ps(s0, s1), _mm256_add_ps(s2, s3)));
    for (; i < n; ++i) {
        const float d = a[i] - b[i];
        sum += d * d;
    }
    return sum;
}

inline float dotSimd(const float* a, const float* b, std::size_t n) noexcept {
    __m256 s0 = _mm256_setzero_ps();
    __m256 s1 = _mm256_setzero_ps();
    __m256 s2 = _mm256_setzero_ps();
    __m256 s3 = _mm256_setzero_ps();
    std::size_t i = 0;
    for (; i + 32 <= n; i += 32) {
        s0 = _mm256_fmadd_ps(_mm256_loadu_ps(a + i), _mm256_loadu_ps(b + i), s0);
        s1 = _mm256_fmadd_ps(_mm256_loadu_ps(a + i + 8), _mm256_loadu_ps(b + i + 8), s1);
        s2 = _mm256_fmadd_ps(_mm256_loadu_ps(a + i + 16), _mm256_loadu_ps(b + i + 16), s2);
        s3 = _mm256_fmadd_ps(_mm256_loadu_ps(a + i + 24), _mm256_loadu_ps(b + i + 24), s3);
    }
    for (; i + 8 <= n; i += 8) {
        s0 = _mm256_fmadd_ps(_mm256_loadu_ps(a + i), _mm256_loadu_ps(b + i), s0);
    }
    float sum = horizontalSum(_mm256_add_ps(_mm256_add_ps(s0, s1), _mm256_add_ps(s2, s3)));
    for (; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

#elif defined(ANN_HAVE_NEON)

inline float l2SqSimd(const float* a, const float* b, std::size_t n) noexcept {
    float32x4_t s0 = vdupq_n_f32(0.0f);
    float32x4_t s1 = vdupq_n_f32(0.0f);
    float32x4_t s2 = vdupq_n_f32(0.0f);
    float32x4_t s3 = vdupq_n_f32(0.0f);
    std::size_t i = 0;
    for (; i + 16 <= n; i += 16) {
        const float32x4_t d0 = vsubq_f32(vld1q_f32(a + i), vld1q_f32(b + i));
        const float32x4_t d1 = vsubq_f32(vld1q_f32(a + i + 4), vld1q_f32(b + i + 4));
        const float32x4_t d2 = vsubq_f32(vld1q_f32(a + i + 8), vld1q_f32(b + i + 8));
        const float32x4_t d3 = vsubq_f32(vld1q_f32(a + i + 12), vld1q_f32(b + i + 12));
        s0 = vfmaq_f32(s0, d0, d0);
        s1 = vfmaq_f32(s1, d1, d1);
        s2 = vfmaq_f32(s2, d2, d2);
        s3 = vfmaq_f32(s3, d3, d3);
    }
    for (; i + 4 <= n; i += 4) {
        const float32x4_t d = vsubq_f32(vld1q_f32(a + i), vld1q_f32(b + i));
        s0 = vfmaq_f32(s0, d, d);
    }
    float sum = vaddvq_f32(vaddq_f32(vaddq_f32(s0, s1), vaddq_f32(s2, s3)));
    for (; i < n; ++i) {
        const float d = a[i] - b[i];
        sum += d * d;
    }
    return sum;
}

inline float dotSimd(const float* a, const float* b, std::size_t n) noexcept {
    float32x4_t s0 = vdupq_n_f32(0.0f);
    float32x4_t s1 = vdupq_n_f32(0.0f);
    float32x4_t s2 = vdupq_n_f32(0.0f);
    float32x4_t s3 = vdupq_n_f32(0.0f);
    std::size_t i = 0;
    for (; i + 16 <= n; i += 16) {
        s0 = vfmaq_f32(s0, vld1q_f32(a + i), vld1q_f32(b + i));
        s1 = vfmaq_f32(s1, vld1q_f32(a + i + 4), vld1q_f32(b + i + 4));
        s2 = vfmaq_f32(s2, vld1q_f32(a + i + 8), vld1q_f32(b + i + 8));
        s3 = vfmaq_f32(s3, vld1q_f32(a + i + 12), vld1q_f32(b + i + 12));
    }
    for (; i + 4 <= n; i += 4) {
        s0 = vfmaq_f32(s0, vld1q_f32(a + i), vld1q_f32(b + i));
    }
    float sum = vaddvq_f32(vaddq_f32(vaddq_f32(s0, s1), vaddq_f32(s2, s3)));
    for (; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

#endif

}  // namespace detail

inline constexpr bool kSimdEnabled =
#if defined(ANN_HAVE_AVX2) || defined(ANN_HAVE_NEON)
    true;
#else
    false;
#endif

// Squared Euclidean distance. Squared because every index only needs the
// ordering; callers that need the metric take the root once at the end.
inline float l2Sq(const float* a, const float* b, std::size_t n) noexcept {
#if defined(ANN_HAVE_AVX2) || defined(ANN_HAVE_NEON)
    return detail::l2SqSimd(a, b, n);
#else
    return detail::l2SqScalar(a, b, n);
#endif
}

inline float dot(const float* a, const float* b, std::size_t n) noexcept {
#if defined(ANN_HAVE_AVX2) || defined(ANN_HAVE_NEON)
    return detail::dotSimd(a, b, n);
#else
    return detail::dotScalar(a, b, n);
#endif
}

inline float squaredNorm(const float* a, std::size_t n) noexcept { return dot(a, a, n); }

inline float norm(const float* a, std::size_t n) noexcept { return std::sqrt(squaredNorm(a, n)); }

// Scales v to unit length in place. A zero vector is left untouched.
inline void normalize(float* v, std::size_t n) noexcept {
    const float len = norm(v, n);
    if (len > 0.0f) {
        const float inv = 1.0f / len;
        for (std::size_t i = 0; i < n; ++i) {
            v[i] *= inv;
        }
    }
}

// Metric policies. Every index is parameterised on one of these. `distance`
// must be "smaller is closer"; `kNormalizeOnInsert` asks the index to store
// unit vectors and to normalise queries, which turns cosine into a dot product.
struct L2 {
    static constexpr bool kNormalizeOnInsert = false;
    static constexpr const char* name() noexcept { return "l2"; }
    static float distance(const float* a, const float* b, std::size_t n) noexcept { return l2Sq(a, b, n); }
};

struct InnerProduct {
    static constexpr bool kNormalizeOnInsert = false;
    static constexpr const char* name() noexcept { return "ip"; }
    static float distance(const float* a, const float* b, std::size_t n) noexcept { return -dot(a, b, n); }
};

struct Cosine {
    static constexpr bool kNormalizeOnInsert = true;
    static constexpr const char* name() noexcept { return "cosine"; }
    static float distance(const float* a, const float* b, std::size_t n) noexcept { return 1.0f - dot(a, b, n); }
};

}  // namespace ann
