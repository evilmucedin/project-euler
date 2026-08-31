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

// Joins a stem and an ending, applying Russian spelling rules at the seam:
// after г/к/х/ж/ч/ш/щ write "и" instead of "ы", and after hushings and "ц"
// write "а"/"у" instead of "я"/"ю".
TWord join(const TWord& stem, const TWord& ending) {
    if (stem.empty() || ending.empty()) {
        return stem + ending;
    }
    TWord fixed = ending;
    const char32_t last = stem.back();
    if ((isVelar(last) || isHushing(last)) && fixed[0] == U'ы') {
        fixed[0] = U'и';
    }
    if (isHushing(last) || last == U'ц') {
        if (fixed[0] == U'я') {
            fixed[0] = U'а';
        } else if (fixed[0] == U'ю') {
            fixed[0] = U'у';
        }
    }
    return stem + fixed;
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

    void add(const TWord& stem, std::initializer_list<const char32_t*> endings) {
        for (const char32_t* ending : endings) {
            add(join(stem, ending));
        }
    }

    std::vector<std::string> take() {
        return std::move(forms_);
    }

private:
    std::vector<std::string> forms_;
    std::unordered_set<std::string> seen_;
};

// --- Noun declension ---

void declineMasculineHard(const TWord& stem, TFormCollector& out) {
    out.add(stem, {U"", U"а", U"у", U"ом", U"е"});
    const char32_t last = stem.empty() ? U' ' : stem.back();
    out.add(stem, {U"ы", isHushing(last) ? U"ей" : U"ов", U"ам", U"ами", U"ах"});
}

void declineMasculineSoftSign(const TWord& stem, TFormCollector& out) {
    out.add(stem, {U"ь", U"я", U"ю", U"ем", U"е"});
    out.add(stem, {U"и", U"ей", U"ям", U"ями", U"ях"});
}

void declineMasculineJ(const TWord& stem, bool endsIj, TFormCollector& out) {
    out.add(stem, {U"й", U"я", U"ю", U"ем"});
    out.add(join(stem, endsIj ? U"и" : U"е"));  // prepositional: о гении / о герое
    out.add(stem, {U"и", U"ев", U"ям", U"ями", U"ях"});
}

void declineFeminineA(const TWord& stem, TFormCollector& out) {
    const char32_t last = stem.empty() ? U' ' : stem.back();
    const bool softInstr = isHushing(last) || last == U'ц';
    out.add(stem, {U"а", U"ы", U"е", U"у", softInstr ? U"ей" : U"ой"});
    out.add(stem, {U"ы", U"", U"ам", U"ами", U"ах"});
}

void declineFeminineJa(const TWord& stem, bool endsIja, TFormCollector& out) {
    if (endsIja) {
        out.add(stem, {U"я", U"и", U"ю", U"ей"});
        out.add(join(stem, U"и"));  // dative/prepositional: армии
        out.add(stem, {U"и", U"й", U"ям", U"ями", U"ях"});
    } else {
        out.add(stem, {U"я", U"и", U"е", U"ю", U"ей"});
        out.add(stem, {U"и", U"ь", U"ям", U"ями", U"ях"});
    }
}

void declineFeminineSoftSign(const TWord& stem, TFormCollector& out) {
    out.add(stem, {U"ь", U"и", U"ью"});
    out.add(stem, {U"и", U"ей", U"ям", U"ями", U"ях"});
}

void declineNeuterO(const TWord& stem, TFormCollector& out) {
    out.add(stem, {U"о", U"а", U"у", U"ом", U"е"});
    out.add(stem, {U"а", U"", U"ам", U"ами", U"ах"});
}

void declineNeuterE(const TWord& stem, bool endsIe, TFormCollector& out) {
    out.add(stem, {U"е", U"я", U"ю", U"ем"});
    out.add(join(stem, endsIe ? U"и" : U"е"));  // prepositional: о здании / о море
    out.add(stem, {U"я", endsIe ? U"й" : U"ей", U"ям", U"ями", U"ях"});
}

bool declineNoun(const TWord& word, TFormCollector& out) {
    const char32_t last = word.back();
    const char32_t prev = word.size() >= 2 ? word[word.size() - 2] : U' ';
    if (last == U'а') {
        declineFeminineA(word.substr(0, word.size() - 1), out);
    } else if (last == U'я') {
        declineFeminineJa(word.substr(0, word.size() - 1), prev == U'и', out);
    } else if (last == U'о') {
        declineNeuterO(word.substr(0, word.size() - 1), out);
    } else if (last == U'е' || last == U'ё') {
        declineNeuterE(word.substr(0, word.size() - 1), prev == U'и', out);
    } else if (last == U'й') {
        declineMasculineJ(word.substr(0, word.size() - 1), prev == U'и', out);
    } else if (last == U'ь') {
        if (isHushing(prev)) {
            declineFeminineSoftSign(word.substr(0, word.size() - 1), out);
        } else {
            declineMasculineSoftSign(word.substr(0, word.size() - 1), out);
        }
    } else if (!isVowel(last)) {
        declineMasculineHard(word, out);
    } else {
        // Vowel endings и/ы/у/ю/э: likely indeclinable or already inflected.
        out.add(word);
    }
    return true;
}

// --- Adjective declension ---

void declineAdjectiveHard(const TWord& nominative, const TWord& stem, TFormCollector& out) {
    out.add(nominative);
    out.add(stem, {U"ого", U"ому", U"ым", U"ом"});   // masculine/neuter oblique
    out.add(stem, {U"ая", U"ой", U"ую"});            // feminine
    out.add(join(stem, U"ое"));                      // neuter nominative
    out.add(stem, {U"ые", U"ых", U"ым", U"ыми"});    // plural
}

void declineAdjectiveSoft(const TWord& stem, TFormCollector& out) {
    out.add(stem, {U"ий", U"его", U"ему", U"им", U"ем"});
    out.add(stem, {U"яя", U"ей", U"юю"});
    out.add(join(stem, U"ее"));
    out.add(stem, {U"ие", U"их", U"им", U"ими"});
}

void declineAdjective(const TWord& word, TFormCollector& out) {
    const TWord stem = word.substr(0, word.size() - 2);
    const char32_t last = stem.empty() ? U' ' : stem.back();
    if (endsWith(word, U"ий") && !isVelar(last)) {
        declineAdjectiveSoft(stem, out);
    } else {
        declineAdjectiveHard(word, stem, out);
    }
}

// --- Verb conjugation ---

void conjugateVerb(const TWord& infinitive, TFormCollector& out) {
    out.add(infinitive);
    const char32_t themeVowel = infinitive[infinitive.size() - 3];
    if (themeVowel == U'и') {
        // Second conjugation: говорить -> говорю, говоришь, ..., говорят.
        const TWord stem = infinitive.substr(0, infinitive.size() - 3);
        out.add(stem, {U"ю", U"ишь", U"ит", U"им", U"ите", U"ят"});
        out.add(stem, {U"ил", U"ила", U"ило", U"или"});
        out.add(stem, {U"и", U"ите"});
    } else if (themeVowel == U'у') {
        // -нуть type: тянуть -> тяну, тянешь, ..., тянул.
        const TWord stem = infinitive.substr(0, infinitive.size() - 3);
        out.add(stem, {U"у", U"ешь", U"ет", U"ем", U"ете", U"ут"});
        out.add(stem, {U"ул", U"ула", U"уло", U"ули"});
        out.add(stem, {U"и", U"ите"});
    } else {
        // Productive first conjugation: читать/гулять/уметь.
        const TWord stem = infinitive.substr(0, infinitive.size() - 2);
        out.add(stem, {U"ю", U"ешь", U"ет", U"ем", U"ете", U"ют"});
        out.add(stem, {U"л", U"ла", U"ло", U"ли"});
        out.add(stem, {U"й", U"йте"});
    }
}

// --- Classification ---

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

// Fully irregular paradigms that the suffix rules cannot produce.
const std::unordered_map<std::string, std::vector<std::string>>& irregularWords() {
    static const std::unordered_map<std::string, std::vector<std::string>> words = {
        {"мать",
         {"мать", "матери", "матерью", "матерей", "матерям", "матерями",
          "матерях"}},
        {"дочь",
         {"дочь", "дочери", "дочерью", "дочерей", "дочерям", "дочерьми",
          "дочерях"}},
        {"путь",
         {"путь", "пути", "путём", "путем", "путей", "путям", "путями",
          "путях"}},
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
        result.forms = it->second;
        return result;
    }

    TFormCollector collector;
    if (feminineSoftSignNouns().count(normalized) != 0) {
        result.partOfSpeech = EPartOfSpeech::Noun;
        declineFeminineSoftSign(letters.substr(0, letters.size() - 1), collector);
    } else if (looksLikeAdjective(letters)) {
        result.partOfSpeech = EPartOfSpeech::Adjective;
        declineAdjective(letters, collector);
    } else if (looksLikeInfinitive(letters)) {
        result.partOfSpeech = EPartOfSpeech::Verb;
        conjugateVerb(letters, collector);
    } else {
        result.partOfSpeech = EPartOfSpeech::Noun;
        declineNoun(letters, collector);
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
