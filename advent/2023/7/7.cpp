#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

int getCardValue(char card) {
    std::map<char, int> cardValues = {
        {'A', 14}, {'K', 13}, {'Q', 12}, {'J', 11}, {'T', 10},
        {'9', 9}, {'8', 8}, {'7', 7}, {'6', 6}, {'5', 5},
        {'4', 4}, {'3', 3}, {'2', 2}
    };
    return cardValues[card];
}

bool compareCards(char card1, char card2) {
    return getCardValue(card1) < getCardValue(card2);
}

bool compareCardsNot(char card1, char card2) {
    return getCardValue(card1) > getCardValue(card2);
}

std::string determineHandType(const std::string& hand) {
    std::string sortedHand = hand;
    std::sort(sortedHand.begin(), sortedHand.end(), compareCards);

    int counts[15] = {0};
    for (char card : sortedHand) {
        counts[getCardValue(card)]++;
    }

    int maxCount = *std::max_element(counts, counts + 15);

    if (maxCount == 5) return "Five of a kind";
    else if (maxCount == 4) return "Four of a kind";
    else if (maxCount == 3 && std::count(counts, counts + 15, 2) == 1) return "Full house";
    else if (maxCount == 3) return "Three of a kind";
    else if (std::count(counts, counts + 15, 2) == 2) return "Two pair";
    else if (std::count(counts, counts + 15, 2) == 1) return "One pair";
    else return "High card";
}

bool compareHands(const std::string& hand1, const std::string& hand2) {
    std::string typeHand1 = determineHandType(hand1);
    std::string typeHand2 = determineHandType(hand2);

    if (typeHand1 != typeHand2) {
        std::map<std::string, int> handOrder = {
            {"Five of a kind", 7}, {"Four of a kind", 6}, {"Full house", 5},
            {"Three of a kind", 4}, {"Two pair", 3}, {"One pair", 2}, {"High card", 1}
        };
        return handOrder[typeHand1] > handOrder[typeHand2];
    } else {
        std::string sortedHand1 = hand1;
        std::string sortedHand2 = hand2;
        // std::sort(sortedHand1.begin(), sortedHand1.end(), compareCards);
        // std::sort(sortedHand2.begin(), sortedHand2.end(), compareCards);

        for (size_t i = 0; i < 5; ++i) {
            if (getCardValue(sortedHand1[i]) != getCardValue(sortedHand2[i])) {
                return getCardValue(sortedHand1[i]) > getCardValue(sortedHand2[i]);
            }
        }
        return false; // Both hands are identical
    }
}


void first() {
    const auto input = readInputLines();
    vector<vector<string>> pairs;
    for (const auto& s: input) {
        auto parts = split(s, ' ');
        assert(parts[0].size() == 5);
        parts.emplace_back(determineHandType(parts[0]));
        pairs.emplace_back(parts);
    }
    sort(pairs.begin(), pairs.end(), [&](const auto& p1, const auto& p2) {
        if (p1[0] != p2[0])
            return !compareHands(p1[0], p2[0]);
        else
            return false;
    });
    cerr << pairs << endl;
    long long result = 0;
    for (long long i = 0; i < pairs.size(); ++i) {
        result += (i + 1)*stoll(pairs[i][1]);
    }
    cout << result << endl;
}

int getCardValueJ(char card) {
    std::map<char, int> cardValues = {
        {'A', 14}, {'K', 13}, {'Q', 12}, {'J', 1}, {'T', 10},
        {'9', 9}, {'8', 8}, {'7', 7}, {'6', 6}, {'5', 5},
        {'4', 4}, {'3', 3}, {'2', 2}
    };
    return cardValues[card];
}

char getCardByValueJ(int card) {
    std::map<int, char> cardValues = {
        {14, 'A'}, {13, 'K'}, {12, 'Q'}, {1, 'J'}, {10, 'T'},
        {9, '9'}, {8, '8'}, {7, '7'}, {6, '6'}, {5, '5'},
        {4, '4'}, {3, '3'}, {2, '2'}
    };
    return cardValues[card];
}

bool compareCardsJ(char card1, char card2) {
    return getCardValueJ(card1) < getCardValueJ(card2);
}

bool compareCardsNotJ(char card1, char card2) {
    return getCardValueJ(card1) > getCardValueJ(card2);
}

std::string determineHandTypeJ(const std::string& hand) {
    std::string sortedHand = hand;
    std::sort(sortedHand.begin(), sortedHand.end(), compareCardsJ);

    int counts[15] = {0};
    for (char card : sortedHand) {
        counts[getCardValueJ(card)]++;
    }

    if (counts[1] == 5) {
        return "Five of a kind";
    }

    int off = std::max_element(counts + 2, counts + 15) - counts;
    for (char& ch: sortedHand) {
        if (ch == 'J') {
            ch = getCardByValueJ(off);
        }
    }

    return determineHandType(sortedHand);

    /*
    int maxCountJ = maxCount + counts[1];

    if (maxCountJ == 5) return "Five of a kind";
    else if (maxCountJ == 4) return "Four of a kind";
    else if (maxCountJ == 3 && ((std::count(counts + 2, counts + 15, 2) == 1) || counts[1] == 2)) return "Full house";
    else if (maxCountJ == 3) return "Three of a kind";
    else if (std::count(counts + 2, counts + 15, 2) == 2 || ((std::count(counts + 2, counts + 15, 2) == 1) && (counts[1] == 2))) return "Two pair";
    else if ((std::count(counts + 2, counts + 15, 2) == 1 || counts[1] == 2) || (counts[1] == 1)) return "One pair";
    else return "High card";
    */
}

bool compareHandsJ(const std::string& hand1, const std::string& hand2) {
    std::string typeHand1 = determineHandTypeJ(hand1);
    std::string typeHand2 = determineHandTypeJ(hand2);

    if (typeHand1 != typeHand2) {
        std::map<std::string, int> handOrder = {
            {"Five of a kind", 7}, {"Four of a kind", 6}, {"Full house", 5},
            {"Three of a kind", 4}, {"Two pair", 3}, {"One pair", 2}, {"High card", 1}
        };
        return handOrder[typeHand1] > handOrder[typeHand2];
    } else {
        std::string sortedHand1 = hand1;
        std::string sortedHand2 = hand2;
        // std::sort(sortedHand1.begin(), sortedHand1.end(), compareCards);
        // std::sort(sortedHand2.begin(), sortedHand2.end(), compareCards);

        for (size_t i = 0; i < 5; ++i) {
            if (getCardValueJ(sortedHand1[i]) != getCardValueJ(sortedHand2[i])) {
                return getCardValueJ(sortedHand1[i]) > getCardValueJ(sortedHand2[i]);
            }
        }
        return false; // Both hands are identical
    }
}


void second() {
    const auto input = readInputLines();
    vector<vector<string>> pairs;
    for (const auto& s: input) {
        auto parts = split(s, ' ');
        assert(parts[0].size() == 5);
        parts.emplace_back(determineHandTypeJ(parts[0]));
        pairs.emplace_back(parts);
    }
    sort(pairs.begin(), pairs.end(), [&](const auto& p1, const auto& p2) {
        if (p1[0] != p2[0])
            return !compareHandsJ(p1[0], p2[0]);
        else
            return false;
    });
    cerr << pairs << endl;
    long long result = 0;
    for (long long i = 0; i < pairs.size(); ++i) {
        result += (i + 1)*stoll(pairs[i][1]);
    }
    cout << result << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}

