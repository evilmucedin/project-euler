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
    // Zaliznyak-style inflection index inferred from the word shape:
    // nouns "м 1".."с 8" (gender + stem type), adjectives "п 1a".."п 4b"
    // (stem type + stress scheme), verbs "гл 1".."гл 5" (conjugation class),
    // "0" for indeclinables. Empty for non-Russian input. Stress schemes are
    // only reported where the word shape determines them (adjectives).
    std::string zaliznyakIndex;
    // All generated forms in lowercase UTF-8, deduplicated; the first element
    // is the normalized input word.
    std::vector<std::string> forms;
};

// Infers the inflection class of a Russian word (UTF-8) following
// A. A. Zaliznyak's classification ("Грамматический словарь русского языка")
// and generates the inflected forms of that same word from the paradigm
// tables of that class: case/number forms for nouns, full agreement forms
// for adjectives, and person/tense forms for verbs. Words that are not
// recognized as inflectable Russian words are returned as a single
// normalized form with EPartOfSpeech::Unknown.
TAnalysis analyze(const std::string& word);

// Basic entry point: all morphological forms of the same word.
std::vector<std::string> getForms(const std::string& word);

const char* partOfSpeechName(EPartOfSpeech partOfSpeech);

}
