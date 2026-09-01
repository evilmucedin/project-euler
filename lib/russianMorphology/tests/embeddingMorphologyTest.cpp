#include "lib/russianMorphology/embeddingMorphology.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

using russianMorphology::EPartOfSpeech;
using russianMorphology::embedding::analyze;
using russianMorphology::embedding::classify;
using russianMorphology::embedding::cosineSimilarity;
using russianMorphology::embedding::getForms;
using russianMorphology::embedding::kEmbeddingDimensions;
using russianMorphology::embedding::TLabeledWord;
using russianMorphology::embedding::trainingLexicon;
using russianMorphology::embedding::wordEmbedding;

testing::AssertionResult containsAll(const std::vector<std::string>& forms,
                                     const std::vector<std::string>& expected) {
    for (const std::string& form : expected) {
        if (std::find(forms.begin(), forms.end(), form) == forms.end()) {
            return testing::AssertionFailure() << "missing form: " << form;
        }
    }
    return testing::AssertionSuccess();
}

TEST(EmbeddingTest, VectorShapeAndNormalization) {
    const std::vector<float> vec = wordEmbedding("стол");
    ASSERT_EQ(static_cast<int>(vec.size()), kEmbeddingDimensions);
    float norm = 0.0f;
    for (float x : vec) {
        norm += x * x;
    }
    EXPECT_NEAR(std::sqrt(norm), 1.0f, 1e-4f);
}

TEST(EmbeddingTest, NonRussianInputHasNoEmbedding) {
    EXPECT_TRUE(wordEmbedding("table").empty());
    EXPECT_TRUE(wordEmbedding("").empty());
    EXPECT_TRUE(wordEmbedding("стол стул").empty());
}

TEST(EmbeddingTest, Deterministic) {
    EXPECT_EQ(wordEmbedding("молоток"), wordEmbedding("молоток"));
    EXPECT_EQ(classify("молоток").zaliznyakIndex,
              classify("молоток").zaliznyakIndex);
}

TEST(EmbeddingTest, CosineSimilarity) {
    const std::vector<float> a = wordEmbedding("читать");
    EXPECT_NEAR(cosineSimilarity(a, a), 1.0f, 1e-4f);
    EXPECT_EQ(cosineSimilarity(a, {}), 0.0f);
    EXPECT_EQ(cosineSimilarity({}, {}), 0.0f);
}

TEST(EmbeddingTest, SimilarWordsCloserThanDissimilar) {
    // Same inflection class (ж 8) should sit closer than a masculine noun of
    // a completely different shape.
    const std::vector<float> salt = wordEmbedding("соль");
    const std::vector<float> honesty = wordEmbedding("честность");
    const std::vector<float> table = wordEmbedding("стол");
    EXPECT_GT(cosineSimilarity(salt, honesty), cosineSimilarity(salt, table));
}

TEST(EmbeddingTest, ClassifyKnownWords) {
    EXPECT_EQ(classify("стол").zaliznyakIndex, "м 1");
    EXPECT_EQ(classify("соль").zaliznyakIndex, "ж 8");
    EXPECT_EQ(classify("читать").zaliznyakIndex, "гл 1");
    EXPECT_EQ(classify("такси").zaliznyakIndex, "0");
    EXPECT_EQ(classify("синий").zaliznyakIndex, "п 2a");
}

TEST(EmbeddingTest, ConfidenceInUnitInterval) {
    const auto prediction = classify("гавань");
    EXPECT_GT(prediction.confidence, 0.0f);
    EXPECT_LE(prediction.confidence, 1.0f);
}

TEST(EmbeddingTest, PredictionIsShapeCompatible) {
    // Whatever class wins, it must be able to inflect the word.
    for (const char* word : {"скатерть", "пудель", "конвой", "манто",
                             "дрожать", "нить", "калий"}) {
        const auto prediction = classify(word);
        ASSERT_FALSE(prediction.zaliznyakIndex.empty()) << word;
        EXPECT_TRUE(russianMorphology::classCompatible(word,
                                                       prediction.zaliznyakIndex))
            << word << " -> " << prediction.zaliznyakIndex;
    }
}

TEST(EmbeddingTest, NonRussianInputAnalysis) {
    EXPECT_EQ(analyze("table").partOfSpeech, EPartOfSpeech::Unknown);
    EXPECT_TRUE(analyze("table").zaliznyakIndex.empty());
    EXPECT_TRUE(classify("table").zaliznyakIndex.empty());
}

TEST(EmbeddingTest, GeneratesParadigms) {
    const auto analysis = analyze("гавань");
    EXPECT_EQ(analysis.partOfSpeech, EPartOfSpeech::Noun);
    EXPECT_TRUE(containsAll(analysis.forms, {"гавани", "гаванью", "гаваней"}));

    EXPECT_TRUE(containsAll(getForms("дрожать"), {"дрожу", "дрожит", "дрожал"}));
    EXPECT_TRUE(containsAll(getForms("прибой"), {"прибоя", "прибоем", "прибои"}));
}

TEST(EmbeddingTest, TrainingAccuracyFloor) {
    // The model must at least have memorized its own small lexicon.
    int correct = 0;
    int total = 0;
    for (const TLabeledWord& entry : trainingLexicon()) {
        ++total;
        if (classify(entry.word).zaliznyakIndex == entry.zaliznyakIndex) {
            ++correct;
        }
    }
    EXPECT_GE(correct, total * 97 / 100)
        << correct << " of " << total << " training words classified correctly";
}

}  // namespace

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
