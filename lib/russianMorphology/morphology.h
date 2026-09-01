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

// Generates the forms of a word from an explicitly given Zaliznyak-style
// index instead of inferring the class from the word shape. Accepts the same
// index notation analyze() reports: "м 1".."м 7", "ж 1".."ж 8", "с 1".."с 7",
// "п 1a".."п 4b", "гл 1".."гл 5", and "0" for indeclinables. For verb classes
// 4/5 the past-tense vowel is read from the infinitive itself. When the index
// is malformed or the word shape cannot inflect as that class (e.g. "ж 8" for
// a word that does not end in -ь), the word is returned as a single
// normalized form with EPartOfSpeech::Unknown. This is the generation half of
// the library; external classifiers (see embeddingMorphology.h) provide the
// class.
TAnalysis analyzeWithClass(const std::string& word, const std::string& zaliznyakIndex);

// True when the word shape can inflect as the given Zaliznyak-style index,
// i.e. analyzeWithClass(word, zaliznyakIndex) would generate a paradigm.
bool classCompatible(const std::string& word, const std::string& zaliznyakIndex);

// Looks the word up in the short shared list of fully irregular paradigms
// (мать, дочь, путь) that no regular inflection class generates; returns true
// and fills `result` when the word is on the list. Part of the generation
// half of the library: every backend consults it before classifying.
bool lookupIrregular(const std::string& word, TAnalysis& result);

const char* partOfSpeechName(EPartOfSpeech partOfSpeech);

}
