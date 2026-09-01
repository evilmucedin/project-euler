#pragma once

// Shared Russian language data: the Zaliznyak paradigm ending tables, the
// closed exception lists, the fully irregular paradigms, and the labeled
// training lexicon. This is the data half of the library, consumed by both
// the rule-based backend (morphology.cpp) and the embedding backend
// (embeddingMorphology.cpp); it changes rarely — only when a paradigm table
// is corrected or a word is added to one of the lists.

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace russianMorphology {
namespace data {

// --- Zaliznyak paradigm ending tables ---
//
// Zaliznyak classifies every inflected word by the type of its stem-final
// sound plus a stress scheme, and the appendix of the dictionary gives one
// ending table per type. The stem types:
//   1  hard paired consonant   (стол, мама, место)
//   2  soft paired consonant   (конь, неделя, поле)
//   3  velar г/к/х             (бык, книга, войско)
//   4  hushing ж/ч/ш/щ         (нож, туча, жилище)
//   5  ц                       (месяц, улица)
//   6  vowel or ь before the ending (герой, шея, платье)
//   7  stem in -и              (гений, армия, здание)
//   8  third declension        (ночь, путь)
// Stress schemes (a..f) are not recoverable without the dictionary; where
// the spelling of an ending depends on stress (ножом vs маршем) the tables
// list an ending as spelling variants separated by '|' (e.g. "ем|ом").

struct TNounEndings {
    // nominative, genitive, dative, accusative, instrumental, prepositional
    const char32_t* singular[6];
    // nominative, genitive, dative, instrumental, prepositional
    // (the accusative matches the nominative or the genitive)
    const char32_t* plural[5];
};

// Ending tables of the masculine stem types 1..7.
const TNounEndings* masculineNounEndings(int type);

// Ending tables of the feminine stem types 1..8; the third declension
// (type 8) differs between hushing and other stems.
const TNounEndings* feminineNounEndings(int type, bool hushingStem);

// Ending tables of the neuter nouns: all о-nouns share one ending row
// regardless of stem type (место, войско, лицо); е-nouns differ by type.
const TNounEndings* neuterNounEndings(bool oNoun, int type);

struct TAdjectiveEndings {
    // nominative, genitive, dative, instrumental, prepositional
    // (the accusative matches the nominative or the genitive)
    const char32_t* masculine[5];
    // nominative, oblique (gen/dat/ins/prep), accusative
    const char32_t* feminine[3];
    // nominative/accusative (other cases match the masculine)
    const char32_t* neuter;
    // nominative, genitive/prepositional, dative, instrumental
    const char32_t* plural[4];
};

// Ending tables of the adjective stem types 1..4; the nominative ending of
// the input word fixes the stress scheme (a: -ый/-ий, b: -ой).
const TAdjectiveEndings* adjectiveEndings(int type, bool endingStressed);

// --- Closed word lists ---

// Class-5 verbs (second conjugation in -ать/-еть/-ять, indistinguishable
// from class 1 by shape) mapped to the vowel of their past-tense suffix.
const std::unordered_map<std::string, char32_t>& classFiveVerbs();

// Common non-verbs whose endings look like an infinitive (vowel + "ть"), and
// other soft-sign feminines that would otherwise be guessed masculine.
const std::unordered_set<std::string>& feminineSoftSignNouns();

// Fully irregular paradigms that no table produces, keyed by the normalized
// word.
struct TIrregularParadigm {
    const char* zaliznyakIndex;
    std::vector<std::string> forms;
};
const std::unordered_map<std::string, TIrregularParadigm>& irregularWords();

// --- Labeled training lexicon ---

// A word labeled with the Zaliznyak-style class (in the notation of
// morphology.h) whose paradigm generates its correct forms.
struct TLabeledWord {
    const char* word;
    const char* zaliznyakIndex;
};

// The labeled lexicon the embedding classifier is trained on, also used by
// tests to check accuracy and train/eval disjointness.
const std::vector<TLabeledWord>& trainingLexicon();

}  // namespace data
}  // namespace russianMorphology
