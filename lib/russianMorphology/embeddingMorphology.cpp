// Embedding-based Russian morphology backend (see embeddingMorphology.h).
//
// The model is a fastText-style supervised classifier: hashed character
// n-grams (n = 2..5 of the boundary-padded word, plus the whole word) each
// own a dense vector; the word embedding is their average; a softmax layer
// over the embedding predicts the Zaliznyak inflection class. Both the
// n-gram vectors and the softmax weights are trained jointly by SGD on the
// labeled lexicon from the shared language-data library (languageData.h).
// Training is deterministic (fixed-seed xorshift,
// fixed shuffle order) and runs in-process on first use in well under a
// second; there are no external files, downloads, or network access.

#include "lib/russianMorphology/embeddingMorphology.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_map>

#include "lib/russianMorphology/utf8.h"

namespace russianMorphology {
namespace embedding {

namespace {

using detail::TWord;
using detail::encodeUtf8;

// --- Model ---

constexpr int kDims = kEmbeddingDimensions;
constexpr int kBuckets = 1 << 13;
constexpr int kMinGram = 2;
constexpr int kMaxGram = 5;
constexpr int kEpochs = 160;
constexpr float kLearningRate = 0.5f;

struct TModel {
    std::vector<std::string> labels;
    std::unordered_map<std::string, int> labelIds;
    std::vector<float> input;   // kBuckets x kDims
    std::vector<float> output;  // labels x kDims
};

struct TXorshift {
    uint64_t state;
    uint64_t next() {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return state;
    }
    // Uniform in [0, bound).
    uint64_t next(uint64_t bound) { return next() % bound; }
    float nextFloat() { return static_cast<float>(next() >> 40) / static_cast<float>(1 << 24); }
};

uint32_t hashGram(const TWord& padded, size_t begin, size_t length) {
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = begin; i < begin + length; ++i) {
        char32_t c = padded[i];
        for (int k = 0; k < 4; ++k) {
            h ^= (c >> (8 * k)) & 0xFF;
            h *= 1099511628211ULL;
        }
    }
    return static_cast<uint32_t>(h % kBuckets);
}

// Hashed character n-grams of the boundary-padded word, plus the whole word.
std::vector<uint32_t> wordFeatures(const TWord& letters) {
    TWord padded;
    padded.push_back(U'<');
    padded += letters;
    padded.push_back(U'>');
    std::vector<uint32_t> features;
    for (size_t n = kMinGram; n <= kMaxGram; ++n) {
        if (padded.size() < n) {
            break;
        }
        for (size_t i = 0; i + n <= padded.size(); ++i) {
            features.push_back(hashGram(padded, i, n));
        }
    }
    if (padded.size() > kMaxGram) {
        features.push_back(hashGram(padded, 0, padded.size()));
    }
    return features;
}

void averageFeatures(const TModel& model, const std::vector<uint32_t>& features,
                     std::vector<float>& hidden) {
    hidden.assign(kDims, 0.0f);
    for (uint32_t feature : features) {
        const float* row = &model.input[feature * kDims];
        for (int d = 0; d < kDims; ++d) {
            hidden[d] += row[d];
        }
    }
    const float scale = 1.0f / static_cast<float>(features.size());
    for (float& value : hidden) {
        value *= scale;
    }
}

void softmax(std::vector<float>& scores) {
    const float top = *std::max_element(scores.begin(), scores.end());
    float sum = 0.0f;
    for (float& score : scores) {
        score = std::exp(score - top);
        sum += score;
    }
    for (float& score : scores) {
        score /= sum;
    }
}

const TModel& getModel() {
    static const TModel model = [] {
        TModel result;
        struct TExample {
            std::vector<uint32_t> features;
            int label;
        };
        std::vector<TExample> examples;
        for (const TLabeledWord& entry : trainingLexicon()) {
            TWord letters;
            detail::normalizeRussianWord(entry.word, letters);
            const auto inserted =
                result.labelIds.emplace(entry.zaliznyakIndex, result.labels.size());
            if (inserted.second) {
                result.labels.push_back(entry.zaliznyakIndex);
            }
            examples.push_back({wordFeatures(letters), inserted.first->second});
        }

        TXorshift rng{20240917ULL};
        result.input.resize(static_cast<size_t>(kBuckets) * kDims);
        for (float& value : result.input) {
            value = (rng.nextFloat() * 2.0f - 1.0f) / kDims;
        }
        result.output.assign(result.labels.size() * kDims, 0.0f);

        const size_t labelCount = result.labels.size();
        std::vector<float> hidden(kDims);
        std::vector<float> scores(labelCount);
        std::vector<float> hiddenGradient(kDims);
        const size_t totalSteps = static_cast<size_t>(kEpochs) * examples.size();
        size_t step = 0;
        for (int epoch = 0; epoch < kEpochs; ++epoch) {
            // Deterministic Fisher-Yates shuffle.
            for (size_t i = examples.size(); i > 1; --i) {
                std::swap(examples[i - 1], examples[rng.next(i)]);
            }
            for (const TExample& example : examples) {
                const float lr =
                    kLearningRate *
                    (1.0f - static_cast<float>(step++) / static_cast<float>(totalSteps));
                averageFeatures(result, example.features, hidden);
                for (size_t l = 0; l < labelCount; ++l) {
                    const float* row = &result.output[l * kDims];
                    float score = 0.0f;
                    for (int d = 0; d < kDims; ++d) {
                        score += row[d] * hidden[d];
                    }
                    scores[l] = score;
                }
                softmax(scores);
                hiddenGradient.assign(kDims, 0.0f);
                for (size_t l = 0; l < labelCount; ++l) {
                    const float gradient =
                        lr * (scores[l] -
                              (static_cast<int>(l) == example.label ? 1.0f : 0.0f));
                    float* row = &result.output[l * kDims];
                    for (int d = 0; d < kDims; ++d) {
                        hiddenGradient[d] += gradient * row[d];
                        row[d] -= gradient * hidden[d];
                    }
                }
                const float scale = 1.0f / static_cast<float>(example.features.size());
                for (uint32_t feature : example.features) {
                    float* row = &result.input[feature * kDims];
                    for (int d = 0; d < kDims; ++d) {
                        row[d] -= hiddenGradient[d] * scale;
                    }
                }
            }
        }
        return result;
    }();
    return model;
}

}  // namespace

std::vector<float> wordEmbedding(const std::string& word) {
    TWord letters;
    if (!detail::normalizeRussianWord(word, letters)) {
        return {};
    }
    std::vector<float> hidden;
    averageFeatures(getModel(), wordFeatures(letters), hidden);
    float norm = 0.0f;
    for (float value : hidden) {
        norm += value * value;
    }
    if (norm > 0.0f) {
        const float scale = 1.0f / std::sqrt(norm);
        for (float& value : hidden) {
            value *= scale;
        }
    }
    return hidden;
}

float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.empty() || a.size() != b.size()) {
        return 0.0f;
    }
    float dot = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    return dot / std::sqrt(normA * normB);
}

TPrediction classify(const std::string& word) {
    TPrediction result;
    TWord letters;
    if (!detail::normalizeRussianWord(word, letters)) {
        return result;
    }
    const TModel& model = getModel();
    std::vector<float> hidden;
    averageFeatures(model, wordFeatures(letters), hidden);
    std::vector<float> scores(model.labels.size());
    for (size_t l = 0; l < model.labels.size(); ++l) {
        const float* row = &model.output[l * kDims];
        float score = 0.0f;
        for (int d = 0; d < kDims; ++d) {
            score += row[d] * hidden[d];
        }
        scores[l] = score;
    }
    softmax(scores);
    // Shape-constrained argmax: only classes whose paradigm can actually
    // inflect this word compete, with the probability renormalized over them.
    const std::string normalized = encodeUtf8(letters);
    float compatibleSum = 0.0f;
    int best = -1;
    for (size_t l = 0; l < model.labels.size(); ++l) {
        if (!classCompatible(normalized, model.labels[l])) {
            continue;
        }
        compatibleSum += scores[l];
        if (best < 0 || scores[l] > scores[best]) {
            best = static_cast<int>(l);
        }
    }
    if (best < 0) {
        result.zaliznyakIndex = "0";
        result.confidence = 1.0f;
        return result;
    }
    result.zaliznyakIndex = model.labels[best];
    result.confidence = compatibleSum > 0.0f ? scores[best] / compatibleSum : 0.0f;
    return result;
}

TAnalysis analyze(const std::string& word) {
    TAnalysis irregular;
    if (lookupIrregular(word, irregular)) {
        return irregular;
    }
    return analyzeWithClass(word, classify(word).zaliznyakIndex);
}

std::vector<std::string> getForms(const std::string& word) {
    return analyze(word).forms;
}

}  // namespace embedding
}  // namespace russianMorphology
