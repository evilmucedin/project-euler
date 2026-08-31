// CLI for manual testing of the Russian morphology library.
//
// Usage:
//   morphologyCli слово [слово...]   # analyze the given words
//   morphologyCli                    # read words from stdin, one per line

#include "lib/russianMorphology/morphology.h"

#include <iostream>
#include <string>

namespace {

void report(const std::string& word) {
    const russianMorphology::TAnalysis analysis = russianMorphology::analyze(word);
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
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            report(argv[i]);
        }
        return 0;
    }
    std::string word;
    while (std::cin >> word) {
        report(word);
    }
    return 0;
}
