// Side-by-side quality comparison of the two morphology backends on the gold
// set from evaluation.h: the rule-based classifier (analyze) and the
// embedding classifier (embedding::analyze). Prints aggregate metrics and,
// with --verbose, every word either backend gets wrong.

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "lib/russianMorphology/embeddingMorphology.h"
#include "lib/russianMorphology/evaluation.h"
#include "lib/russianMorphology/morphology.h"

namespace {

using russianMorphology::evaluation::TScore;
using russianMorphology::evaluation::TWordResult;

void printScore(const char* name, const TScore& score) {
    std::printf("%-12s %3d/%3d (%5.1f%%)   %3d/%3d (%5.1f%%)   %3d/%3d (%5.1f%%)\n",
                name,
                score.posCorrect, score.total,
                100.0 * score.posCorrect / score.total,
                score.classCorrect, score.total,
                100.0 * score.classCorrect / score.total,
                score.paradigmCorrect, score.total,
                100.0 * score.paradigmCorrect / score.total);
}

void printMistakes(const char* name, const std::vector<TWordResult>& details) {
    std::printf("\n%s mistakes:\n", name);
    bool any = false;
    for (const TWordResult& result : details) {
        if (result.classCorrect && result.paradigmCorrect) {
            continue;
        }
        any = true;
        std::printf("  %-12s gold %-5s got %-5s", result.gold->word,
                    result.gold->zaliznyakIndex, result.predictedIndex.c_str());
        if (!result.missingForms.empty()) {
            std::printf("  missing:");
            for (const std::string& form : result.missingForms) {
                std::printf(" %s", form.c_str());
            }
        }
        if (!result.forbiddenHits.empty()) {
            std::printf("  wrong:");
            for (const std::string& form : result.forbiddenHits) {
                std::printf(" %s", form.c_str());
            }
        }
        std::printf("\n");
    }
    if (!any) {
        std::printf("  (none)\n");
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    const bool verbose = argc > 1 && std::strcmp(argv[1], "--verbose") == 0;

    std::vector<TWordResult> ruleDetails;
    std::vector<TWordResult> embeddingDetails;
    const TScore ruleScore =
        russianMorphology::evaluation::evaluate(&russianMorphology::analyze,
                                                &ruleDetails);
    const TScore embeddingScore = russianMorphology::evaluation::evaluate(
        &russianMorphology::embedding::analyze, &embeddingDetails);

    std::printf("Gold set: %d words (disjoint from the embedding training "
                "lexicon)\n\n", ruleScore.total);
    std::printf("%-12s %-20s %-20s %-20s\n", "backend", "part of speech",
                "inflection class", "paradigm");
    printScore("rules", ruleScore);
    printScore("embeddings", embeddingScore);

    if (verbose) {
        printMistakes("rules", ruleDetails);
        printMistakes("embeddings", embeddingDetails);
    }
    return 0;
}
