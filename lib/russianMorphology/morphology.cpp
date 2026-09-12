// Rule-based Russian morphology following A. A. Zaliznyak's classification
// ("Грамматический словарь русского языка"). A word is mapped to an
// inflection class — gender + stem type for nouns, stem type + stress scheme
// for adjectives, conjugation class for verbs — and its forms are generated
// from the explicit ending table of that class, mirroring the declension and
// conjugation tables of the dictionary's appendix. There is no word list, so
// the class is inferred from the shape of the word alone.

#include "lib/russianMorphology/morphology.h"

#include <initializer_list>
#include <unordered_map>
#include <unordered_set>

namespace russianMorphology {

namespace {

using TWord = std::u32string;

// --- UTF-8 <-> code points ---

TWord decodeUtf8(const std::string& text) {
    TWord result;
    size_t i = 0;
    while (i < text.size()) {
        const unsigned char byte = static_cast<unsigned char>(text[i]);
        char32_t code = 0;
        size_t length = 1;
        if (byte < 0x80) {
            code = byte;
        } else if ((byte & 0xE0) == 0xC0) {
            code = byte & 0x1F;
            length = 2;
        } else if ((byte & 0xF0) == 0xE0) {
            code = byte & 0x0F;
            length = 3;
        } else if ((byte & 0xF8) == 0xF0) {
            code = byte & 0x07;
            length = 4;
        } else {
            // Invalid lead byte: keep it as-is so the word round-trips.
            code = byte;
        }
        if (i + length > text.size()) {
            code = byte;
            length = 1;
        }
        for (size_t k = 1; k < length; ++k) {
            const unsigned char cont = static_cast<unsigned char>(text[i + k]);
            if ((cont & 0xC0) != 0x80) {
                code = byte;
                length = 1;
                break;
            }
            code = (code << 6) | (cont & 0x3F);
        }
        result.push_back(code);
        i += length;
    }
    return result;
}

std::string encodeUtf8(const TWord& word) {
    std::string result;
    for (char32_t code : word) {
        if (code < 0x80) {
            result.push_back(static_cast<char>(code));
        } else if (code < 0x800) {
            result.push_back(static_cast<char>(0xC0 | (code >> 6)));
            result.push_back(static_cast<char>(0x80 | (code & 0x3F)));
        } else if (code < 0x10000) {
            result.push_back(static_cast<char>(0xE0 | (code >> 12)));
            result.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (code & 0x3F)));
        } else {
            result.push_back(static_cast<char>(0xF0 | (code >> 18)));
            result.push_back(static_cast<char>(0x80 | ((code >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (code & 0x3F)));
        }
    }
    return result;
}

// --- Russian alphabet helpers ---

char32_t toLowerRussian(char32_t c) {
    if (c >= U'А' && c <= U'Я') {
        return c + (U'а' - U'А');
    }
    if (c == U'Ё') {
        return U'ё';
    }
    return c;
}

bool isRussianLetter(char32_t c) {
    return (c >= U'а' && c <= U'я') || c == U'ё';
}

bool isVowel(char32_t c) {
    switch (c) {
        case U'а': case U'е': case U'ё': case U'и': case U'о':
        case U'у': case U'ы': case U'э': case U'ю': case U'я':
            return true;
        default:
            return false;
    }
}

bool isHushing(char32_t c) {
    return c == U'ж' || c == U'ч' || c == U'ш' || c == U'щ';
}

bool isVelar(char32_t c) {
    return c == U'г' || c == U'к' || c == U'х';
}

bool endsWith(const TWord& word, const TWord& suffix) {
    return word.size() >= suffix.size() &&
           word.compare(word.size() - suffix.size(), suffix.size(), suffix) == 0;
}

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
// list both variants.

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

// --- Noun declension tables ---

struct TNounEndings {
    // nominative, genitive, dative, accusative, instrumental, prepositional
    const char32_t* singular[6];
    // nominative, genitive, dative, instrumental, prepositional
    // (the accusative matches the nominative or the genitive)
    const char32_t* plural[5];
};

const TNounEndings* masculineEndings(int type) {
    static const TNounEndings type1 = {{U"", U"а", U"у", U"", U"ом", U"е"},
                                       {U"ы", U"ов", U"ам", U"ами", U"ах"}};
    static const TNounEndings type2 = {{U"ь", U"я", U"ю", U"ь", U"ем", U"е"},
                                       {U"и", U"ей", U"ям", U"ями", U"ях"}};
    static const TNounEndings type3 = {{U"", U"а", U"у", U"", U"ом", U"е"},
                                       {U"и", U"ов", U"ам", U"ами", U"ах"}};
    static const TNounEndings type4 = {{U"", U"а", U"у", U"", U"ем|ом", U"е"},
                                       {U"и", U"ей", U"ам", U"ами", U"ах"}};
    static const TNounEndings type5 = {{U"", U"а", U"у", U"", U"ем|ом", U"е"},
                                       {U"ы", U"ев|ов", U"ам", U"ами", U"ах"}};
    static const TNounEndings type6 = {{U"й", U"я", U"ю", U"й", U"ем", U"е"},
                                       {U"и", U"ев", U"ям", U"ями", U"ях"}};
    static const TNounEndings type7 = {{U"й", U"я", U"ю", U"й", U"ем", U"и"},
                                       {U"и", U"ев", U"ям", U"ями", U"ях"}};
    switch (type) {
        case 2: return &type2;
        case 3: return &type3;
        case 4: return &type4;
        case 5: return &type5;
        case 6: return &type6;
        case 7: return &type7;
        default: return &type1;
    }
}

const TNounEndings* feminineEndings(int type, char32_t stemLast) {
    static const TNounEndings type1 = {{U"а", U"ы", U"е", U"у", U"ой", U"е"},
                                       {U"ы", U"", U"ам", U"ами", U"ах"}};
    static const TNounEndings type2 = {{U"я", U"и", U"е", U"ю", U"ей", U"е"},
                                       {U"и", U"ь", U"ям", U"ями", U"ях"}};
    static const TNounEndings type3 = {{U"а", U"и", U"е", U"у", U"ой", U"е"},
                                       {U"и", U"", U"ам", U"ами", U"ах"}};
    static const TNounEndings type4 = {{U"а", U"и", U"е", U"у", U"ей|ой", U"е"},
                                       {U"и", U"", U"ам", U"ами", U"ах"}};
    static const TNounEndings type5 = {{U"а", U"ы", U"е", U"у", U"ей|ой", U"е"},
                                       {U"ы", U"", U"ам", U"ами", U"ах"}};
    static const TNounEndings type6 = {{U"я", U"и", U"е", U"ю", U"ей", U"е"},
                                       {U"и", U"й", U"ям", U"ями", U"ях"}};
    static const TNounEndings type7 = {{U"я", U"и", U"и", U"ю", U"ей", U"и"},
                                       {U"и", U"й", U"ям", U"ями", U"ях"}};
    static const TNounEndings type8 = {{U"ь", U"и", U"и", U"ь", U"ью", U"и"},
                                       {U"и", U"ей", U"ям", U"ями", U"ях"}};
    static const TNounEndings type8Hushing = {{U"ь", U"и", U"и", U"ь", U"ью", U"и"},
                                              {U"и", U"ей", U"ам", U"ами", U"ах"}};
    switch (type) {
        case 2: return &type2;
        case 3: return &type3;
        case 4: return &type4;
        case 5: return &type5;
        case 6: return &type6;
        case 7: return &type7;
        case 8: return isHushing(stemLast) ? &type8Hushing : &type8;
        default: return &type1;
    }
}

const TNounEndings* neuterEndings(char32_t nominative, int type) {
    // All о-nouns share one ending row regardless of stem type (место,
    // войско, лицо); е-nouns differ by type.
    static const TNounEndings hardO = {{U"о", U"а", U"у", U"о", U"ом", U"е"},
                                       {U"а", U"", U"ам", U"ами", U"ах"}};
    static const TNounEndings type2 = {{U"е", U"я", U"ю", U"е", U"ем", U"е"},
                                       {U"я", U"ей", U"ям", U"ями", U"ях"}};
    static const TNounEndings type4 = {{U"е", U"а", U"у", U"е", U"ем", U"е"},
                                       {U"а", U"", U"ам", U"ами", U"ах"}};
    static const TNounEndings type5 = {{U"е", U"а", U"у", U"е", U"ем|ом", U"е"},
                                       {U"а", U"", U"ам", U"ами", U"ах"}};
    static const TNounEndings type6 = {{U"е", U"я", U"ю", U"е", U"ем", U"е"},
                                       {U"я", U"ев", U"ям", U"ями", U"ях"}};
    static const TNounEndings type7 = {{U"е", U"я", U"ю", U"е", U"ем", U"и"},
                                       {U"я", U"й", U"ям", U"ями", U"ях"}};
    if (nominative == U'о') {
        return &hardO;
    }
    switch (type) {
        case 4: return &type4;
        case 5: return &type5;
        case 6: return &type6;
        case 7: return &type7;
        default: return &type2;
    }
}

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
    result.endings = feminineEndings(8, result.stem.back());
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
        result.endings = feminineEndings(result.type, prev);
    } else if (last == U'я') {
        result.gender = "ж";
        result.type = prev == U'и' ? 7 : (prev == U'ь' || isVowel(prev) ? 6 : 2);
        result.endings = feminineEndings(result.type, prev);
    } else if (last == U'о') {
        result.gender = "с";
        result.type = consonantStemType(prev);
        result.endings = neuterEndings(last, result.type);
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
        result.endings = neuterEndings(U'е', result.type);
    } else if (last == U'й') {
        result.gender = "м";
        result.type = prev == U'и' ? 7 : 6;
        result.endings = masculineEndings(result.type);
    } else if (last == U'ь') {
        if (isHushing(prev)) {
            return feminineThirdDeclension(word);
        }
        result.gender = "м";
        result.type = 2;
        result.endings = masculineEndings(2);
    } else if (!isVowel(last)) {
        result.gender = "м";
        result.type = consonantStemType(last);
        result.stem = word;
        result.endings = masculineEndings(result.type);
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

// --- Adjective declension tables ---

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

// Adjective stem types use the same numbering; the nominative ending of the
// input word fixes the stress scheme (a: -ый/-ий, b: -ой).
const TAdjectiveEndings* adjectiveEndings(int type, bool endingStressed) {
    static const TAdjectiveEndings type1a = {{U"ый", U"ого", U"ому", U"ым", U"ом"},
                                             {U"ая", U"ой", U"ую"},
                                             U"ое",
                                             {U"ые", U"ых", U"ым", U"ыми"}};
    static const TAdjectiveEndings type1b = {{U"ой", U"ого", U"ому", U"ым", U"ом"},
                                             {U"ая", U"ой", U"ую"},
                                             U"ое",
                                             {U"ые", U"ых", U"ым", U"ыми"}};
    static const TAdjectiveEndings type2a = {{U"ий", U"его", U"ему", U"им", U"ем"},
                                             {U"яя", U"ей", U"юю"},
                                             U"ее",
                                             {U"ие", U"их", U"им", U"ими"}};
    static const TAdjectiveEndings type3a = {{U"ий", U"ого", U"ому", U"им", U"ом"},
                                             {U"ая", U"ой", U"ую"},
                                             U"ое",
                                             {U"ие", U"их", U"им", U"ими"}};
    static const TAdjectiveEndings type3b = {{U"ой", U"ого", U"ому", U"им", U"ом"},
                                             {U"ая", U"ой", U"ую"},
                                             U"ое",
                                             {U"ие", U"их", U"им", U"ими"}};
    static const TAdjectiveEndings type4a = {{U"ий", U"его", U"ему", U"им", U"ем"},
                                             {U"ая", U"ей", U"ую"},
                                             U"ее",
                                             {U"ие", U"их", U"им", U"ими"}};
    static const TAdjectiveEndings type4b = {{U"ой", U"ого", U"ому", U"им", U"ом"},
                                             {U"ая", U"ой", U"ую"},
                                             U"ое",
                                             {U"ие", U"их", U"им", U"ими"}};
    switch (type) {
        case 2: return &type2a;
        case 3: return endingStressed ? &type3b : &type3a;
        case 4: return endingStressed ? &type4b : &type4a;
        default: return endingStressed ? &type1b : &type1a;
    }
}

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

// Class-5 verbs mapped to the vowel of their past-tense suffix.
const std::unordered_map<std::string, char32_t>& classFiveVerbs() {
    static const std::unordered_map<std::string, char32_t> verbs = {
        {"слышать", U'а'}, {"дышать", U'а'}, {"держать", U'а'}, {"лежать", U'а'},
        {"молчать", U'а'}, {"кричать", U'а'}, {"стучать", U'а'}, {"звучать", U'а'},
        {"спать", U'а'},
        {"видеть", U'е'}, {"смотреть", U'е'}, {"сидеть", U'е'}, {"лететь", U'е'},
        {"гореть", U'е'}, {"терпеть", U'е'}, {"вертеть", U'е'}, {"обидеть", U'е'},
        {"зависеть", U'е'}, {"ненавидеть", U'е'},
        {"стоять", U'я'},
    };
    return verbs;
}

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

// Common non-verbs whose endings look like an infinitive (vowel + "ть"), and
// other soft-sign feminines that would otherwise be guessed masculine.
const std::unordered_set<std::string>& feminineSoftSignNouns() {
    static const std::unordered_set<std::string> words = {
        "сеть", "кровать", "печать", "память", "площадь", "лошадь",
        "тетрадь", "ткань", "дверь", "соль", "боль", "жизнь", "любовь",
        "осень", "очередь", "медаль",
    };
    return words;
}

// Fully irregular paradigms that no table produces.
struct TIrregular {
    const char* index;
    std::vector<std::string> forms;
};

const std::unordered_map<std::string, TIrregular>& irregularWords() {
    static const std::unordered_map<std::string, TIrregular> words = {
        {"мать",
         {"ж 8",
          {"мать", "матери", "матерью", "матерей", "матерям", "матерями",
           "матерях"}}},
        {"дочь",
         {"ж 8",
          {"дочь", "дочери", "дочерью", "дочерей", "дочерям", "дочерьми",
           "дочерях"}}},
        {"путь",
         {"м 8",
          {"путь", "пути", "путём", "путем", "путей", "путям", "путями",
           "путях"}}},
    };
    return words;
}

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
        result.zaliznyakIndex = it->second.index;
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
