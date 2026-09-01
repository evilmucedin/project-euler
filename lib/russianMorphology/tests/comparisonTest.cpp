// Quality comparison between the rule-based and the embedding-based backend
// on the shared gold set, with thresholds locking in the measured quality so
// regressions in either backend or in the shared paradigm tables fail loudly.

#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "lib/russianMorphology/embeddingMorphology.h"
#include "lib/russianMorphology/evaluation.h"
#include "lib/russianMorphology/morphology.h"

namespace {

using russianMorphology::evaluation::evaluate;
using russianMorphology::evaluation::goldWords;
using russianMorphology::evaluation::TGoldWord;
using russianMorphology::evaluation::TScore;
using russianMorphology::evaluation::TWordResult;

std::string describeMistakes(const std::vector<TWordResult>& details) {
    std::string out;
    for (const TWordResult& result : details) {
        if (result.classCorrect && result.paradigmCorrect) {
            continue;
        }
        out += result.gold->word;
        out += " (gold ";
        out += result.gold->zaliznyakIndex;
        out += ", got ";
        out += result.predictedIndex;
        out += ") ";
    }
    return out;
}

TEST(ComparisonTest, GoldSetIsDisjointFromTrainingLexicon) {
    // The evaluation must measure generalization: no gold word may appear in
    // the embedding backend's training data.
    std::set<std::string> trainingWords;
    for (const auto& entry : russianMorphology::embedding::trainingLexicon()) {
        trainingWords.insert(entry.word);
    }
    for (const TGoldWord& gold : goldWords()) {
        EXPECT_EQ(trainingWords.count(gold.word), 0u)
            << gold.word << " is in both the gold set and the training lexicon";
    }
}

TEST(ComparisonTest, GoldSetIsWellFormed) {
    std::set<std::string> seen;
    for (const TGoldWord& gold : goldWords()) {
        EXPECT_TRUE(seen.insert(gold.word).second)
            << "duplicate gold word: " << gold.word;
        EXPECT_FALSE(gold.requiredForms.empty()) << gold.word;
    }
    EXPECT_GE(goldWords().size(), 90u);
}

TEST(ComparisonTest, RuleBackendQualityFloor) {
    std::vector<TWordResult> details;
    const TScore score = evaluate(&russianMorphology::analyze, &details);
    // The rule backend misses feminine -ь nouns outside its exception list,
    // -ий/-ой nouns, indeclinables, and class-5 verbs outside its closed
    // list; these floors document its measured level.
    EXPECT_GE(100 * score.posCorrect, 75 * score.total) << describeMistakes(details);
    EXPECT_GE(100 * score.classCorrect, 55 * score.total)
        << describeMistakes(details);
    EXPECT_GE(100 * score.paradigmCorrect, 55 * score.total)
        << describeMistakes(details);
}

TEST(ComparisonTest, EmbeddingBackendQualityFloor) {
    std::vector<TWordResult> details;
    const TScore score =
        evaluate(&russianMorphology::embedding::analyze, &details);
    EXPECT_GE(100 * score.posCorrect, 90 * score.total) << describeMistakes(details);
    EXPECT_GE(100 * score.classCorrect, 80 * score.total)
        << describeMistakes(details);
    EXPECT_GE(100 * score.paradigmCorrect, 80 * score.total)
        << describeMistakes(details);
}

TEST(ComparisonTest, EmbeddingBackendBeatsRulesOnHeldOutWords) {
    const TScore ruleScore = evaluate(&russianMorphology::analyze);
    const TScore embeddingScore =
        evaluate(&russianMorphology::embedding::analyze);
    EXPECT_GT(embeddingScore.classCorrect, ruleScore.classCorrect);
    EXPECT_GT(embeddingScore.paradigmCorrect, ruleScore.paradigmCorrect);
    EXPECT_GE(embeddingScore.posCorrect, ruleScore.posCorrect);
}

}  // namespace

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
