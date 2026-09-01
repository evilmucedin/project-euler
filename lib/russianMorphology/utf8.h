#pragma once

// Internal UTF-8 and Russian-alphabet helpers shared by the rule-based and
// the embedding-based backends. Not part of the public API.

#include <string>

namespace russianMorphology {
namespace detail {

using TWord = std::u32string;

inline TWord decodeUtf8(const std::string& text) {
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

inline std::string encodeUtf8(const TWord& word) {
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

inline char32_t toLowerRussian(char32_t c) {
    if (c >= U'А' && c <= U'Я') {
        return c + (U'а' - U'А');
    }
    if (c == U'Ё') {
        return U'ё';
    }
    return c;
}

inline bool isRussianLetter(char32_t c) {
    return (c >= U'а' && c <= U'я') || c == U'ё';
}

inline bool isVowel(char32_t c) {
    switch (c) {
        case U'а': case U'е': case U'ё': case U'и': case U'о':
        case U'у': case U'ы': case U'э': case U'ю': case U'я':
            return true;
        default:
            return false;
    }
}

inline bool isHushing(char32_t c) {
    return c == U'ж' || c == U'ч' || c == U'ш' || c == U'щ';
}

inline bool isVelar(char32_t c) {
    return c == U'г' || c == U'к' || c == U'х';
}

inline bool endsWith(const TWord& word, const TWord& suffix) {
    return word.size() >= suffix.size() &&
           word.compare(word.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Lowercases the input and returns true when every character is a Russian
// letter (i.e. the input is a single normalizable Russian word).
inline bool normalizeRussianWord(const std::string& text, TWord& letters) {
    letters = decodeUtf8(text);
    for (char32_t& c : letters) {
        c = toLowerRussian(c);
    }
    if (letters.empty()) {
        return false;
    }
    for (char32_t c : letters) {
        if (!isRussianLetter(c)) {
            return false;
        }
    }
    return true;
}

}  // namespace detail
}  // namespace russianMorphology
