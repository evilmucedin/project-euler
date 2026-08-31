#pragma once

#include <string>
#include <vector>

namespace russianMorphology {

enum class EPartOfSpeech {
    Unknown,
    Noun,
    Adjective,
    Verb,
};

struct TAnalysis {
    EPartOfSpeech partOfSpeech = EPartOfSpeech::Unknown;
    // All generated forms in lowercase UTF-8, deduplicated; the first element
    // is the normalized input word.
    std::vector<std::string> forms;
};

// Guesses the paradigm of a Russian word (UTF-8) from its ending and
// generates the inflected forms of that same word: case/number forms for
// nouns, full agreement forms for adjectives, and person/tense forms for
// verbs. Words that are not recognized as inflectable Russian words are
// returned as a single normalized form with EPartOfSpeech::Unknown.
TAnalysis analyze(const std::string& word);

// Basic entry point: all morphological forms of the same word.
std::vector<std::string> getForms(const std::string& word);

const char* partOfSpeechName(EPartOfSpeech partOfSpeech);

}
