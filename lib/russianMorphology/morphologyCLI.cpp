// CLI for manual testing of the Russian morphology library.
//
// Usage:
//   morphologyCli слово [слово...]                # rule-based backend
//   morphologyCli --embedding слово [слово...]    # embedding backend
//   morphologyCli [--embedding]                   # read words from stdin

#include "lib/russianMorphology/embeddingMorphology.h"
#include "lib/russianMorphology/morphology.h"

#include <cstring>
#include <iostream>
#include <string>

namespace {

void report(const std::string& word, bool useEmbedding) {
    const russianMorphology::TAnalysis analysis =
        useEmbedding ? russianMorphology::embedding::analyze(word)
                     : russianMorphology::analyze(word);
    std::cout << word << " (" << russianMorphology::partOfSpeechName(analysis.partOfSpeech);
    if (!analysis.zaliznyakIndex.empty()) {
        std::cout << ", " << analysis.zaliznyakIndex;
    }
    std::cout << "):" << std::endl;
    for (const std::string& form : analysis.forms) {
        std::cout << "  " << form << std::endl;
    }
}

}  // namespace

int main(int argc, char** argv) {
    int firstWord = 1;
    bool useEmbedding = false;
    if (argc > 1 && std::strcmp(argv[1], "--embedding") == 0) {
        useEmbedding = true;
        firstWord = 2;
    }
    if (firstWord < argc) {
        for (int i = firstWord; i < argc; ++i) {
            report(argv[i], useEmbedding);
        }
        return 0;
    }
    std::string word;
    while (std::cin >> word) {
        report(word, useEmbedding);
    }
    return 0;
}
