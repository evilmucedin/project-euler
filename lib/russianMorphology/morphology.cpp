// Rule-based Russian morphology following A. A. Zaliznyak's classification
// ("Грамматический словарь русского языка"). A word is mapped to an
// inflection class — gender + stem type for nouns, stem type + stress scheme
// for adjectives, conjugation class for verbs — and its forms are generated
// from the explicit ending table of that class, mirroring the declension and
// conjugation tables of the dictionary's appendix. There is no word list, so
// the class is inferred from the shape of the word alone.

#include "lib/russianMorphology/morphology.h"

#include <initializer_list>
#include <unordered_set>

#include "lib/russianMorphology/languageData.h"
#include "lib/russianMorphology/utf8.h"

namespace russianMorphology {

namespace {

using data::adjectiveEndings;
using data::classFiveVerbs;
using data::feminineNounEndings;
using data::feminineSoftSignNouns;
using data::irregularWords;
using data::masculineNounEndings;
using data::neuterNounEndings;
using data::TAdjectiveEndings;
using data::TNounEndings;
using detail::TWord;
using detail::decodeUtf8;
using detail::encodeUtf8;
using detail::endsWith;
using detail::isHushing;
using detail::isRussianLetter;
using detail::isVelar;
using detail::isVowel;
using detail::toLowerRussian;

class TFormCollector {
public:
    void add(const TWord& form) {
        if (form.empty()) {
            return;
        }
        std::string encoded = encodeUtf8(form);
        if (seen_.insert(encoded).second) {
            forms_.push_back(std::move(encoded));
        }
    }

    // An ending may list spelling variants separated by '|' (e.g. "ем|ом"
    // where the choice depends on a stress scheme the shape of the word does
    // not determine); every variant is added.
    void add(const TWord& stem, const char32_t* ending) {
        TWord current = stem;
        for (const char32_t* p = ending;; ++p) {
            if (*p == U'|' || *p == 0) {
                add(current);
                current = stem;
                if (*p == 0) {
                    break;
                }
            } else {
                current.push_back(*p);
            }
        }
    }

    void add(const TWord& stem, std::initializer_list<const char32_t*> endings) {
        for (const char32_t* ending : endings) {
            add(stem, ending);
        }
    }

    std::vector<std::string> take() {
        return std::move(forms_);
    }

private:
    std::vector<std::string> forms_;
    std::unordered_set<std::string> seen_;
};

// --- Zaliznyak stem types ---
//
// The stem-type inventory and the ending table of each type live in
// languageData.h. The stem type of a consonant-final stem follows from the
// stem-final sound alone:

int consonantStemType(char32_t c) {
    if (isVelar(c)) {
        return 3;
    }
    if (isHushing(c)) {
        return 4;
    }
    if (c == U'ц') {
        return 5;
    }
    return 1;
}

// --- Noun declension ---
// The ending tables live in languageData.h; classification below maps a word
// shape to gender + stem type and picks the table.

struct TNounClass {
    bool declinable = false;
    const char* gender = "";  // "м", "ж", "с"
    int type = 0;
    TWord stem;
    const TNounEndings* endings = nullptr;
};

TNounClass feminineThirdDeclension(const TWord& word) {
    TNounClass result;
    result.declinable = true;
    result.gender = "ж";
    result.type = 8;
    result.stem = word.substr(0, word.size() - 1);
    result.endings = feminineNounEndings(8, result.stem.back());
    return result;
}

TNounClass classifyNoun(const TWord& word) {
    const char32_t last = word.back();
    const char32_t prev = word.size() >= 2 ? word[word.size() - 2] : U' ';
    TNounClass result;
    result.declinable = true;
    result.stem = word.substr(0, word.size() - 1);
    if (last == U'а') {
        result.gender = "ж";
        result.type = consonantStemType(prev);
        result.endings = feminineNounEndings(result.type, prev);
    } else if (last == U'я') {
        result.gender = "ж";
        result.type = prev == U'и' ? 7 : (prev == U'ь' || isVowel(prev) ? 6 : 2);
        result.endings = feminineNounEndings(result.type, prev);
    } else if (last == U'о') {
        result.gender = "с";
        result.type = consonantStemType(prev);
        result.endings = neuterNounEndings(last, result.type);
    } else if (last == U'е' || last == U'ё') {
        result.gender = "с";
        if (prev == U'и') {
            result.type = 7;
        } else if (isHushing(prev)) {
            result.type = 4;
        } else if (prev == U'ц') {
            result.type = 5;
        } else if (prev == U'ь' || isVowel(prev)) {
            result.type = 6;
        } else {
            result.type = 2;
        }
        result.endings = neuterNounEndings(last, result.type);
    } else if (last == U'й') {
        result.gender = "м";
        result.type = prev == U'и' ? 7 : 6;
        result.endings = masculineNounEndings(result.type);
    } else if (last == U'ь') {
        if (isHushing(prev)) {
            return feminineThirdDeclension(word);
        }
        result.gender = "м";
        result.type = 2;
        result.endings = masculineNounEndings(2);
    } else if (!isVowel(last)) {
        result.gender = "м";
        result.type = consonantStemType(last);
        result.stem = word;
        result.endings = masculineNounEndings(result.type);
    } else {
        // Final и/ы/у/ю/э: indeclinable or already inflected.
        result.declinable = false;
    }
    return result;
}

void declineNoun(const TNounClass& nounClass, TFormCollector& out) {
    for (const char32_t* ending : nounClass.endings->singular) {
        out.add(nounClass.stem, ending);
    }
    for (const char32_t* ending : nounClass.endings->plural) {
        out.add(nounClass.stem, ending);
    }
}

// --- Adjective declension ---
// Adjective stem types use the same numbering; the nominative ending of the
// input word fixes the stress scheme (a: -ый/-ий, b: -ой).

struct TAdjectiveClass {
    int type = 1;
    bool endingStressed = false;
    TWord stem;
    const TAdjectiveEndings* endings = nullptr;
};

TAdjectiveClass classifyAdjective(const TWord& word) {
    TAdjectiveClass result;
    result.stem = word.substr(0, word.size() - 2);
    const char32_t stemLast = result.stem.back();
    result.endingStressed = endsWith(word, U"ой");
    if (isVelar(stemLast)) {
        result.type = 3;
    } else if (isHushing(stemLast)) {
        result.type = 4;
    } else if (endsWith(word, U"ий")) {
        result.type = 2;
    } else {
        result.type = 1;
    }
    result.endings = adjectiveEndings(result.type, result.endingStressed);
    return result;
}

void declineAdjective(const TAdjectiveClass& adjectiveClass, TFormCollector& out) {
    const TAdjectiveEndings& endings = *adjectiveClass.endings;
    for (const char32_t* ending : endings.masculine) {
        out.add(adjectiveClass.stem, ending);
    }
    for (const char32_t* ending : endings.feminine) {
        out.add(adjectiveClass.stem, ending);
    }
    out.add(adjectiveClass.stem, endings.neuter);
    for (const char32_t* ending : endings.plural) {
        out.add(adjectiveClass.stem, ending);
    }
}

// --- Verb conjugation ---
//
// The Zaliznyak conjugation classes that are inferable from the infinitive
// without a dictionary:
//   1  -ать/-ять/-еть, present in -аю/-яю/-ею  (читать, гулять, уметь)
//   2  -овать/-евать, present in -ую/-юю       (рисовать, танцевать, воевать)
//   3  -нуть                                   (тянуть)
//   4  -ить, second conjugation                (говорить, ходить)
//   5  -ать/-еть/-ять second conjugation: a closed list (слышать, видеть,
//      стоять, ...), since the shape does not distinguish them from class 1.
// The regular first-person-singular alternations of classes 4/5 are applied
// (ходить -> хожу, любить -> люблю, простить -> прощу).

int verbClass(const TWord& word, const std::string& normalized) {
    if (classFiveVerbs().count(normalized) != 0) {
        return 5;
    }
    if (word.size() >= 6 && (endsWith(word, U"овать") || endsWith(word, U"евать"))) {
        return 2;
    }
    if (word.size() >= 5 && endsWith(word, U"нуть")) {
        return 3;
    }
    if (endsWith(word, U"ить")) {
        return 4;
    }
    return 1;
}

// Standard first-person-singular consonant alternation of classes 4/5.
TWord mutateFirstSingularStem(const TWord& stem) {
    if (endsWith(stem, U"ст")) {
        return stem.substr(0, stem.size() - 2) + U'щ';
    }
    TWord result = stem;
    switch (stem.back()) {
        case U'д': case U'з':
            result.back() = U'ж';
            break;
        case U'т':
            result.back() = U'ч';
            break;
        case U'с':
            result.back() = U'ш';
            break;
        case U'б': case U'п': case U'в': case U'ф': case U'м':
            result.push_back(U'л');
            break;
        default:
            break;
    }
    return result;
}

void conjugateFirstConjugation(const TWord& presentStem, const TWord& pastStem,
                               TFormCollector& out) {
    out.add(presentStem, {U"ю", U"ешь", U"ет", U"ем", U"ете", U"ют"});
    out.add(pastStem, {U"л", U"ла", U"ло", U"ли"});
    out.add(presentStem, {U"й", U"йте"});
}

void conjugateSecondConjugation(const TWord& stem, char32_t pastVowel,
                                TFormCollector& out) {
    TWord firstSingular = mutateFirstSingularStem(stem);
    const char32_t back = firstSingular.back();
    firstSingular.push_back(isHushing(back) || back == U'ц' ? U'у' : U'ю');
    out.add(firstSingular);
    out.add(stem, {U"ишь", U"ит", U"им", U"ите"});
    out.add(stem, isHushing(stem.back()) ? U"ат" : U"ят");
    TWord pastStem = stem;
    pastStem.push_back(pastVowel);
    out.add(pastStem, {U"л", U"ла", U"ло", U"ли"});
    if (isVowel(stem.back())) {
        out.add(stem, {U"й", U"йте"});
    } else {
        out.add(stem, {U"и", U"ите"});
    }
}

void conjugateVerb(const TWord& infinitive, int cls, char32_t classFivePastVowel,
                   TFormCollector& out) {
    switch (cls) {
        case 2: {
            // рисова-ть -> рису-ю, ноче-ва-ть -> ночу-ю, вое-ва-ть -> вою-ю.
            TWord presentStem = infinitive.substr(0, infinitive.size() - 5);
            presentStem.push_back(isVowel(presentStem.back()) ? U'ю' : U'у');
            conjugateFirstConjugation(presentStem,
                                      infinitive.substr(0, infinitive.size() - 2), out);
            break;
        }
        case 3: {
            const TWord stem = infinitive.substr(0, infinitive.size() - 3);
            out.add(stem, {U"у", U"ешь", U"ет", U"ем", U"ете", U"ут"});
            out.add(stem, {U"ул", U"ула", U"уло", U"ули"});
            out.add(stem, {U"и", U"ите"});
            break;
        }
        case 4:
            conjugateSecondConjugation(infinitive.substr(0, infinitive.size() - 3),
                                       U'и', out);
            break;
        case 5:
            conjugateSecondConjugation(infinitive.substr(0, infinitive.size() - 3),
                                       classFivePastVowel, out);
            break;
        default: {
            const TWord stem = infinitive.substr(0, infinitive.size() - 2);
            conjugateFirstConjugation(stem, stem, out);
            break;
        }
    }
}

// --- Classification helpers ---

bool looksLikeInfinitive(const TWord& word) {
    if (word.size() < 4 || !endsWith(word, U"ть")) {
        return false;
    }
    const char32_t themeVowel = word[word.size() - 3];
    switch (themeVowel) {
        case U'а': case U'я': case U'е': case U'и': case U'у':
            return true;
        default:
            return false;
    }
}

bool looksLikeAdjective(const TWord& word) {
    if (word.size() < 4) {
        return false;
    }
    if (endsWith(word, U"ый") || endsWith(word, U"ий")) {
        return true;
    }
    // "-ой" is shared with common nouns (герой, покой); only long words are
    // confidently adjectives (большой, дорогой).
    return endsWith(word, U"ой") && word.size() >= 6;
}

std::string makeIndex(const char* prefix, int type, const char* scheme = "") {
    std::string result = prefix;
    result += ' ';
    result += static_cast<char>('0' + type);
    result += scheme;
    return result;
}

// --- Generation from an explicitly given inflection class ---
//
// The paradigm tables above are keyed by the shape of the word, so a class
// index is only usable when the word shape fits it: "ж 8" needs a word in
// "-ь", "гл 2" needs "-овать"/"-евать", and so on. These helpers check that
// compatibility and extract the stem, which lets an external classifier (the
// embedding backend) pick any class it likes among the shape-compatible ones.

enum class EGender { Masculine, Feminine, Neuter };

struct TParsedIndex {
    EPartOfSpeech partOfSpeech = EPartOfSpeech::Unknown;
    EGender gender = EGender::Masculine;
    int type = 0;
    bool endingStressed = false;
    bool indeclinable = false;
};

bool parseZaliznyakIndex(const std::string& index, TParsedIndex& out) {
    if (index == "0") {
        out.partOfSpeech = EPartOfSpeech::Noun;
        out.indeclinable = true;
        return true;
    }
    const size_t space = index.find(' ');
    if (space == std::string::npos || space + 1 >= index.size()) {
        return false;
    }
    const std::string prefix = index.substr(0, space);
    const std::string rest = index.substr(space + 1);
    if (rest[0] < '1' || rest[0] > '8') {
        return false;
    }
    out.type = rest[0] - '0';
    if (prefix == "м" || prefix == "ж" || prefix == "с") {
        if (rest.size() != 1) {
            return false;
        }
        out.partOfSpeech = EPartOfSpeech::Noun;
        out.gender = prefix == "м" ? EGender::Masculine
                                   : (prefix == "ж" ? EGender::Feminine : EGender::Neuter);
        return true;
    }
    if (prefix == "п") {
        if (rest.size() != 2 || (rest[1] != 'a' && rest[1] != 'b')) {
            return false;
        }
        out.partOfSpeech = EPartOfSpeech::Adjective;
        out.endingStressed = rest[1] == 'b';
        return true;
    }
    if (prefix == "гл") {
        if (rest.size() != 1 || out.type > 5) {
            return false;
        }
        out.partOfSpeech = EPartOfSpeech::Verb;
        return true;
    }
    return false;
}

// Fills stem/endings when the word shape can decline as gender + stem type.
bool nounParadigm(const TWord& word, EGender gender, int type, TWord& stem,
                  const TNounEndings*& endings) {
    if (word.size() < 2) {
        return false;
    }
    const char32_t last = word.back();
    const char32_t prev = word[word.size() - 2];
    switch (gender) {
        case EGender::Masculine:
            if (last == U'ь') {
                // Hushing + ь nouns (ночь, дочь, мышь) are always feminine.
                if (type != 2 || isHushing(prev)) {
                    return false;
                }
                stem = word.substr(0, word.size() - 1);
            } else if (last == U'й') {
                if (type != (prev == U'и' ? 7 : 6)) {
                    return false;
                }
                stem = word.substr(0, word.size() - 1);
            } else if (!isVowel(last)) {
                if (type != consonantStemType(last)) {
                    return false;
                }
                stem = word;
            } else {
                return false;
            }
            endings = masculineNounEndings(type);
            return true;
        case EGender::Feminine:
            if (last == U'а') {
                if (type != consonantStemType(prev)) {
                    return false;
                }
            } else if (last == U'я') {
                if (type != (prev == U'и' ? 7
                                          : (prev == U'ь' || isVowel(prev) ? 6 : 2))) {
                    return false;
                }
            } else if (last == U'ь') {
                if (type != 8) {
                    return false;
                }
            } else {
                return false;
            }
            stem = word.substr(0, word.size() - 1);
            endings = feminineNounEndings(type, stem.back());
            return true;
        case EGender::Neuter: {
            int shapeType;
            if (last == U'о') {
                shapeType = consonantStemType(prev);
            } else if (last == U'е' || last == U'ё') {
                if (prev == U'и') {
                    shapeType = 7;
                } else if (isHushing(prev)) {
                    shapeType = 4;
                } else if (prev == U'ц') {
                    shapeType = 5;
                } else if (prev == U'ь' || isVowel(prev)) {
                    shapeType = 6;
                } else {
                    shapeType = 2;
                }
            } else {
                return false;
            }
            if (type != shapeType) {
                return false;
            }
            stem = word.substr(0, word.size() - 1);
            endings = neuterNounEndings(last, type);
            return true;
        }
    }
    return false;
}

bool adjectiveParadigm(const TWord& word, int type, bool endingStressed, TWord& stem,
                       const TAdjectiveEndings*& endings) {
    if (word.size() < 4 ||
        (!endsWith(word, U"ый") && !endsWith(word, U"ий") && !endsWith(word, U"ой"))) {
        return false;
    }
    const TAdjectiveClass derived = classifyAdjective(word);
    if (derived.type != type || derived.endingStressed != endingStressed) {
        return false;
    }
    stem = derived.stem;
    endings = derived.endings;
    return true;
}

bool verbShapeCompatible(const TWord& word, int cls) {
    if (word.size() < 4 || !endsWith(word, U"ть")) {
        return false;
    }
    const char32_t theme = word[word.size() - 3];
    switch (cls) {
        case 1:
        case 5:
            return theme == U'а' || theme == U'я' || theme == U'е';
        case 2:
            return word.size() >= 6 &&
                   (endsWith(word, U"овать") || endsWith(word, U"евать"));
        case 3:
            return word.size() >= 5 && endsWith(word, U"нуть");
        case 4:
            return theme == U'и';
        default:
            return false;
    }
}

bool indexCompatible(const TWord& letters, const TParsedIndex& parsed) {
    if (parsed.indeclinable) {
        return true;
    }
    TWord stem;
    switch (parsed.partOfSpeech) {
        case EPartOfSpeech::Noun: {
            const TNounEndings* endings = nullptr;
            return nounParadigm(letters, parsed.gender, parsed.type, stem, endings);
        }
        case EPartOfSpeech::Adjective: {
            const TAdjectiveEndings* endings = nullptr;
            return adjectiveParadigm(letters, parsed.type, parsed.endingStressed, stem,
                                     endings);
        }
        case EPartOfSpeech::Verb:
            return verbShapeCompatible(letters, parsed.type);
        default:
            return false;
    }
}

}  // namespace

TAnalysis analyze(const std::string& word) {
    TWord letters = decodeUtf8(word);
    for (char32_t& c : letters) {
        c = toLowerRussian(c);
    }

    TAnalysis result;
    if (letters.empty()) {
        return result;
    }
    for (char32_t c : letters) {
        if (!isRussianLetter(c)) {
            result.forms.push_back(encodeUtf8(letters));
            return result;
        }
    }

    const std::string normalized = encodeUtf8(letters);
    const auto& irregular = irregularWords();
    if (auto it = irregular.find(normalized); it != irregular.end()) {
        result.partOfSpeech = EPartOfSpeech::Noun;
        result.zaliznyakIndex = it->second.zaliznyakIndex;
        result.forms = it->second.forms;
        return result;
    }

    TFormCollector collector;
    collector.add(letters);  // The normalized input is always the first form.
    if (feminineSoftSignNouns().count(normalized) != 0) {
        result.partOfSpeech = EPartOfSpeech::Noun;
        result.zaliznyakIndex = makeIndex("ж", 8);
        declineNoun(feminineThirdDeclension(letters), collector);
    } else if (looksLikeAdjective(letters)) {
        result.partOfSpeech = EPartOfSpeech::Adjective;
        const TAdjectiveClass adjectiveClass = classifyAdjective(letters);
        result.zaliznyakIndex = makeIndex("п", adjectiveClass.type,
                                          adjectiveClass.endingStressed ? "b" : "a");
        declineAdjective(adjectiveClass, collector);
    } else if (looksLikeInfinitive(letters)) {
        result.partOfSpeech = EPartOfSpeech::Verb;
        const int cls = verbClass(letters, normalized);
        char32_t pastVowel = U'и';
        if (cls == 5) {
            pastVowel = classFiveVerbs().at(normalized);
        }
        result.zaliznyakIndex = makeIndex("гл", cls);
        conjugateVerb(letters, cls, pastVowel, collector);
    } else {
        result.partOfSpeech = EPartOfSpeech::Noun;
        const TNounClass nounClass = classifyNoun(letters);
        if (nounClass.declinable) {
            result.zaliznyakIndex = makeIndex(nounClass.gender, nounClass.type);
            declineNoun(nounClass, collector);
        } else {
            result.zaliznyakIndex = "0";
        }
    }
    result.forms = collector.take();
    return result;
}

TAnalysis analyzeWithClass(const std::string& word, const std::string& zaliznyakIndex) {
    TWord letters;
    TAnalysis result;
    const bool isRussian = detail::normalizeRussianWord(word, letters);
    if (!letters.empty()) {
        result.forms.push_back(encodeUtf8(letters));
    }
    TParsedIndex parsed;
    if (!isRussian || !parseZaliznyakIndex(zaliznyakIndex, parsed) ||
        !indexCompatible(letters, parsed)) {
        return result;
    }
    result.partOfSpeech = parsed.partOfSpeech;
    result.zaliznyakIndex = zaliznyakIndex;
    if (parsed.indeclinable) {
        return result;
    }
    TFormCollector collector;
    collector.add(letters);
    TWord stem;
    switch (parsed.partOfSpeech) {
        case EPartOfSpeech::Noun: {
            const TNounEndings* endings = nullptr;
            nounParadigm(letters, parsed.gender, parsed.type, stem, endings);
            TNounClass nounClass;
            nounClass.stem = stem;
            nounClass.endings = endings;
            declineNoun(nounClass, collector);
            break;
        }
        case EPartOfSpeech::Adjective: {
            TAdjectiveClass adjectiveClass;
            adjectiveParadigm(letters, parsed.type, parsed.endingStressed,
                              adjectiveClass.stem, adjectiveClass.endings);
            declineAdjective(adjectiveClass, collector);
            break;
        }
        case EPartOfSpeech::Verb: {
            // For classes 4/5 the past-tense vowel is the theme vowel of the
            // infinitive itself (говор-и-ть -> говорил, слыш-а-ть -> слышал).
            const char32_t themeVowel = letters[letters.size() - 3];
            conjugateVerb(letters, parsed.type, themeVowel, collector);
            break;
        }
        default:
            break;
    }
    result.forms = collector.take();
    return result;
}

bool classCompatible(const std::string& word, const std::string& zaliznyakIndex) {
    TWord letters;
    TParsedIndex parsed;
    return detail::normalizeRussianWord(word, letters) &&
           parseZaliznyakIndex(zaliznyakIndex, parsed) && indexCompatible(letters, parsed);
}

bool lookupIrregular(const std::string& word, TAnalysis& result) {
    TWord letters;
    if (!detail::normalizeRussianWord(word, letters)) {
        return false;
    }
    const auto& irregular = irregularWords();
    const auto it = irregular.find(encodeUtf8(letters));
    if (it == irregular.end()) {
        return false;
    }
    result.partOfSpeech = EPartOfSpeech::Noun;
    result.zaliznyakIndex = it->second.zaliznyakIndex;
    result.forms = it->second.forms;
    return true;
}

std::vector<std::string> getForms(const std::string& word) {
    return analyze(word).forms;
}

const char* partOfSpeechName(EPartOfSpeech partOfSpeech) {
    switch (partOfSpeech) {
        case EPartOfSpeech::Noun:
            return "noun";
        case EPartOfSpeech::Adjective:
            return "adjective";
        case EPartOfSpeech::Verb:
            return "verb";
        case EPartOfSpeech::Unknown:
        default:
            return "unknown";
    }
}

}  // namespace russianMorphology
