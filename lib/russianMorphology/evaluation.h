#pragma once

// Shared quality-evaluation harness for the two morphology backends: a gold
// set of Russian words with their correct inflection class and key forms,
// plus scoring. Used by the comparison CLI (morphologyCompare) and by the
// comparison test.

#include <string>
#include <vector>

#include "lib/russianMorphology/morphology.h"

namespace russianMorphology {
namespace evaluation {

struct TGoldWord {
    const char* word;
    EPartOfSpeech partOfSpeech;
    // The class of the library's paradigm inventory that produces the correct
    // forms of the word ("м 8" for путь, which only the irregular list covers).
    const char* zaliznyakIndex;
    // Forms every correct analysis must generate...
    std::vector<const char*> requiredForms;
    // ...and characteristic wrong forms a misclassification would generate.
    std::vector<const char*> forbiddenForms;
};

// The gold set. Disjoint from the embedding backend's training lexicon (a
// test enforces this), so it measures generalization, not memorization. The
// rule-based backend's built-in exception lists (мать, сеть, ...) are part of
// that backend and stay in play for it.
const std::vector<TGoldWord>& goldWords();

struct TScore {
    int total = 0;
    int posCorrect = 0;       // part of speech matches
    int classCorrect = 0;     // Zaliznyak index matches
    int paradigmCorrect = 0;  // all required forms present and no forbidden form
};

struct TWordResult {
    const TGoldWord* gold = nullptr;
    std::string predictedIndex;
    bool posCorrect = false;
    bool classCorrect = false;
    bool paradigmCorrect = false;
    std::vector<std::string> missingForms;
    std::vector<std::string> forbiddenHits;
};

using TAnalyzer = TAnalysis (*)(const std::string&);

// Runs the analyzer over the gold set. Per-word outcomes are appended to
// `details` when it is non-null.
TScore evaluate(TAnalyzer analyzer, std::vector<TWordResult>* details = nullptr);

}  // namespace evaluation
}  // namespace russianMorphology
