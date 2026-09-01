#pragma once

// Embedding-based Russian morphology backend.
//
// Instead of hand-written classification rules, a word is mapped to a dense
// vector by averaging learned character-n-gram embeddings (the fastText
// subword scheme: hashed n-grams of the boundary-padded word), and a softmax
// layer over that vector predicts the word's Zaliznyak inflection class. The
// model is trained on the small labeled lexicon embedded in this library the
// first time it is used — deterministically, in-process, with no files,
// downloads, or network access — and the predicted class drives the same
// paradigm tables as the rule-based backend (analyzeWithClass()).
//
// Prediction is shape-constrained: only classes whose paradigm is compatible
// with the shape of the word (classCompatible()) compete in the argmax, so
// the classifier spends its capacity on the genuinely ambiguous decisions —
// сеть/читать (noun vs infinitive), соль/руль (feminine vs masculine -ь),
// гений/синий (noun vs adjective -ий), кино/окно (indeclinable vs neuter),
// слышать/слушать (second vs first conjugation).

#include <string>
#include <vector>

#include "lib/russianMorphology/languageData.h"
#include "lib/russianMorphology/morphology.h"

namespace russianMorphology {
namespace embedding {

struct TPrediction {
    // Zaliznyak-style class index in the same notation analyze() reports;
    // empty when the input is not a single Russian word.
    std::string zaliznyakIndex;
    // Softmax probability of the predicted class among the shape-compatible
    // classes of this word.
    float confidence = 0.0f;
};

// The dense embedding of a word (L2-normalized, kEmbeddingDimensions floats);
// empty for input that is not a single Russian word. Morphologically similar
// words (same inflection class) land close in this space.
inline constexpr int kEmbeddingDimensions = 48;
std::vector<float> wordEmbedding(const std::string& word);

// Cosine similarity of two embeddings; 0 when either is empty.
float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);

// Predicts the inflection class of the word (shape-constrained argmax over
// the softmax distribution).
TPrediction classify(const std::string& word);

// Drop-in equivalents of russianMorphology::analyze()/getForms() that use the
// embedding classifier to pick the inflection class and the shared paradigm
// tables to generate the forms.
TAnalysis analyze(const std::string& word);
std::vector<std::string> getForms(const std::string& word);

// The labeled lexicon the classifier is trained on lives in the shared
// language-data library (languageData.h); re-exported here so that tests and
// the evaluation harness can keep referring to embedding::trainingLexicon().
using data::TLabeledWord;
using data::trainingLexicon;

}  // namespace embedding
}  // namespace russianMorphology
