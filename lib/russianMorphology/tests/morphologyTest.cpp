#include "lib/russianMorphology/morphology.h"

#include <algorithm>
#include <string>
#include <vector>

#include "gtest/gtest.h"

using russianMorphology::EPartOfSpeech;
using russianMorphology::TAnalysis;
using russianMorphology::analyze;
using russianMorphology::getForms;

namespace {

testing::AssertionResult containsAll(const std::vector<std::string>& forms,
                                     const std::vector<std::string>& expected) {
    for (const std::string& form : expected) {
        if (std::find(forms.begin(), forms.end(), form) == forms.end()) {
            testing::AssertionResult failure = testing::AssertionFailure();
            failure << "missing form \"" << form << "\"; got:";
            for (const std::string& f : forms) {
                failure << " " << f;
            }
            return failure;
        }
    }
    return testing::AssertionSuccess();
}

}  // namespace

TEST(RussianMorphology, EmptyAndNonRussianInput) {
    EXPECT_TRUE(getForms("").empty());
    EXPECT_EQ(getForms("hello"), std::vector<std::string>{"hello"});
    EXPECT_EQ(getForms("мама42"), std::vector<std::string>{"мама42"});
    EXPECT_EQ(analyze("hello").partOfSpeech, EPartOfSpeech::Unknown);
}

TEST(RussianMorphology, NormalizesCaseAndPutsInputFirst) {
    const std::vector<std::string> forms = getForms("МАМА");
    ASSERT_FALSE(forms.empty());
    EXPECT_EQ(forms.front(), "мама");
}

TEST(RussianMorphology, FormsAreDeduplicated) {
    for (const char* word : {"мама", "стол", "красный", "читать"}) {
        std::vector<std::string> forms = getForms(word);
        std::vector<std::string> sorted = forms;
        std::sort(sorted.begin(), sorted.end());
        EXPECT_TRUE(std::adjacent_find(sorted.begin(), sorted.end()) == sorted.end())
            << "duplicate forms for " << word;
    }
}

TEST(RussianMorphology, FeminineHardNoun) {
    const TAnalysis analysis = analyze("мама");
    EXPECT_EQ(analysis.partOfSpeech, EPartOfSpeech::Noun);
    EXPECT_TRUE(containsAll(
        analysis.forms,
        {"мама", "мамы", "маме", "маму", "мамой", "мам", "мамам", "мамами", "мамах"}));
}

TEST(RussianMorphology, FeminineNounVelarSpellingRule) {
    // After "г" the genitive singular is "книги", never "книгы".
    const std::vector<std::string> forms = getForms("книга");
    EXPECT_TRUE(containsAll(forms, {"книга", "книги", "книге", "книгу", "книгой", "книгам"}));
    EXPECT_EQ(std::count(forms.begin(), forms.end(), "книгы"), 0);
}

TEST(RussianMorphology, FeminineSoftNoun) {
    EXPECT_TRUE(containsAll(
        getForms("неделя"),
        {"неделя", "недели", "неделе", "неделю", "неделей", "недель", "неделям"}));
}

TEST(RussianMorphology, FeminineIjaNoun) {
    EXPECT_TRUE(containsAll(
        getForms("армия"),
        {"армия", "армии", "армию", "армией", "армий", "армиям", "армиях"}));
}

TEST(RussianMorphology, MasculineHardNoun) {
    const TAnalysis analysis = analyze("стол");
    EXPECT_EQ(analysis.partOfSpeech, EPartOfSpeech::Noun);
    EXPECT_TRUE(containsAll(
        analysis.forms,
        {"стол", "стола", "столу", "столом", "столе", "столы", "столов",
         "столам", "столами", "столах"}));
}

TEST(RussianMorphology, MasculineHushingNoun) {
    // "нож": plural "ножи" (not "ножы"), genitive plural "ножей" (not "ножов").
    const std::vector<std::string> forms = getForms("нож");
    EXPECT_TRUE(containsAll(forms, {"нож", "ножа", "ножи", "ножей", "ножам"}));
    EXPECT_EQ(std::count(forms.begin(), forms.end(), "ножы"), 0);
    EXPECT_EQ(std::count(forms.begin(), forms.end(), "ножов"), 0);
}

TEST(RussianMorphology, MasculineSoftSignNoun) {
    EXPECT_TRUE(containsAll(
        getForms("конь"),
        {"конь", "коня", "коню", "конем", "коне", "кони", "коней", "коням"}));
}

TEST(RussianMorphology, MasculineJNoun) {
    EXPECT_TRUE(containsAll(
        getForms("герой"),
        {"герой", "героя", "герою", "героем", "герое", "герои", "героев", "героями"}));
}

TEST(RussianMorphology, NeuterNouns) {
    EXPECT_TRUE(containsAll(
        getForms("место"),
        {"место", "места", "месту", "местом", "месте", "мест", "местам", "местах"}));
    EXPECT_TRUE(containsAll(
        getForms("море"),
        {"море", "моря", "морю", "морем", "морей", "морям", "морях"}));
    EXPECT_TRUE(containsAll(
        getForms("здание"),
        {"здание", "здания", "зданию", "зданием", "здании", "зданий", "зданиям"}));
}

TEST(RussianMorphology, FeminineThirdDeclension) {
    // Hushing + soft sign is guessed as a third-declension feminine.
    EXPECT_TRUE(containsAll(
        getForms("ночь"),
        {"ночь", "ночи", "ночью", "ночей", "ночам", "ночами", "ночах"}));
    // Listed exception: "тетрадь" would otherwise be guessed masculine.
    EXPECT_TRUE(containsAll(getForms("тетрадь"), {"тетрадь", "тетради", "тетрадью"}));
}

TEST(RussianMorphology, HardAdjective) {
    const TAnalysis analysis = analyze("красный");
    EXPECT_EQ(analysis.partOfSpeech, EPartOfSpeech::Adjective);
    EXPECT_TRUE(containsAll(
        analysis.forms,
        {"красный", "красного", "красному", "красным", "красном", "красная",
         "красной", "красную", "красное", "красные", "красных", "красными"}));
}

TEST(RussianMorphology, VelarAdjectiveSpellingRule) {
    EXPECT_TRUE(containsAll(
        getForms("русский"),
        {"русский", "русского", "русскому", "русским", "русская", "русскую",
         "русское", "русские", "русских", "русскими"}));
}

TEST(RussianMorphology, SoftAdjective) {
    EXPECT_TRUE(containsAll(
        getForms("синий"),
        {"синий", "синего", "синему", "синим", "синем", "синяя", "синей",
         "синюю", "синее", "синие", "синих", "синими"}));
}

TEST(RussianMorphology, HushingAdjective) {
    EXPECT_TRUE(containsAll(
        getForms("хороший"),
        {"хороший", "хорошего", "хорошему", "хорошим", "хорошая", "хорошую",
         "хорошее", "хорошие", "хороших"}));
}

TEST(RussianMorphology, OjAdjective) {
    EXPECT_TRUE(containsAll(
        getForms("большой"),
        {"большой", "большого", "большому", "большим", "большая", "большую",
         "большое", "большие", "больших"}));
}

TEST(RussianMorphology, FirstConjugationVerb) {
    const TAnalysis analysis = analyze("читать");
    EXPECT_EQ(analysis.partOfSpeech, EPartOfSpeech::Verb);
    EXPECT_TRUE(containsAll(
        analysis.forms,
        {"читать", "читаю", "читаешь", "читает", "читаем", "читаете", "читают",
         "читал", "читала", "читало", "читали", "читай", "читайте"}));
}

TEST(RussianMorphology, SecondConjugationVerb) {
    EXPECT_TRUE(containsAll(
        getForms("говорить"),
        {"говорить", "говорю", "говоришь", "говорит", "говорим", "говорите",
         "говорят", "говорил", "говорила", "говорило", "говорили", "говори"}));
}

TEST(RussianMorphology, SecondConjugationHushingStem) {
    // "лечить": hushing stem forces "лечу"/"лечат" (not "лечю"/"лечят").
    const std::vector<std::string> forms = getForms("лечить");
    EXPECT_TRUE(containsAll(forms, {"лечу", "лечит", "лечат", "лечил"}));
    EXPECT_EQ(std::count(forms.begin(), forms.end(), "лечю"), 0);
    EXPECT_EQ(std::count(forms.begin(), forms.end(), "лечят"), 0);
}

TEST(RussianMorphology, NutVerb) {
    EXPECT_TRUE(containsAll(
        getForms("тянуть"),
        {"тянуть", "тяну", "тянешь", "тянет", "тянут", "тянул", "тянула", "тяни"}));
}

TEST(RussianMorphology, InfinitiveLookalikeNouns) {
    // "сеть" and "кровать" end in vowel + "ть" but are nouns.
    const TAnalysis net = analyze("сеть");
    EXPECT_EQ(net.partOfSpeech, EPartOfSpeech::Noun);
    EXPECT_TRUE(containsAll(net.forms, {"сеть", "сети", "сетью", "сетям"}));
    EXPECT_EQ(analyze("кровать").partOfSpeech, EPartOfSpeech::Noun);
}

TEST(RussianMorphology, ZaliznyakIndex) {
    EXPECT_EQ(analyze("стол").zaliznyakIndex, "м 1");
    EXPECT_EQ(analyze("книга").zaliznyakIndex, "ж 3");
    EXPECT_EQ(analyze("нож").zaliznyakIndex, "м 4");
    EXPECT_EQ(analyze("улица").zaliznyakIndex, "ж 5");
    EXPECT_EQ(analyze("герой").zaliznyakIndex, "м 6");
    EXPECT_EQ(analyze("армия").zaliznyakIndex, "ж 7");
    EXPECT_EQ(analyze("здание").zaliznyakIndex, "с 7");
    EXPECT_EQ(analyze("ночь").zaliznyakIndex, "ж 8");
    EXPECT_EQ(analyze("путь").zaliznyakIndex, "м 8");
    EXPECT_EQ(analyze("красный").zaliznyakIndex, "п 1a");
    EXPECT_EQ(analyze("русский").zaliznyakIndex, "п 3a");
    EXPECT_EQ(analyze("хороший").zaliznyakIndex, "п 4a");
    EXPECT_EQ(analyze("большой").zaliznyakIndex, "п 4b");
    EXPECT_EQ(analyze("читать").zaliznyakIndex, "гл 1");
    EXPECT_EQ(analyze("рисовать").zaliznyakIndex, "гл 2");
    EXPECT_EQ(analyze("тянуть").zaliznyakIndex, "гл 3");
    EXPECT_EQ(analyze("говорить").zaliznyakIndex, "гл 4");
    EXPECT_EQ(analyze("видеть").zaliznyakIndex, "гл 5");
    EXPECT_EQ(analyze("hello").zaliznyakIndex, "");
}

TEST(RussianMorphology, IndeclinableNoun) {
    const TAnalysis analysis = analyze("такси");
    EXPECT_EQ(analysis.zaliznyakIndex, "0");
    EXPECT_EQ(analysis.forms, std::vector<std::string>{"такси"});
}

TEST(RussianMorphology, TseStemNouns) {
    // Type 5: "ы" after "ц" in the nominative plural, "ев"/"ов" genitive.
    EXPECT_TRUE(containsAll(
        getForms("месяц"),
        {"месяц", "месяца", "месяцу", "месяцем", "месяцы", "месяцев", "месяцам"}));
    const std::vector<std::string> forms = getForms("улица");
    EXPECT_TRUE(containsAll(forms, {"улица", "улицы", "улице", "улицу", "улицей",
                                    "улиц", "улицам", "улицами", "улицах"}));
    EXPECT_EQ(std::count(forms.begin(), forms.end(), "улици"), 0);
}

TEST(RussianMorphology, OvaEvaVerbs) {
    // Class 2: -ова-/-ева- is replaced by -у- in the present tense.
    const std::vector<std::string> drawForms = getForms("рисовать");
    EXPECT_TRUE(containsAll(
        drawForms,
        {"рисовать", "рисую", "рисуешь", "рисует", "рисуем", "рисуете",
         "рисуют", "рисовал", "рисовала", "рисовали", "рисуй", "рисуйте"}));
    EXPECT_EQ(std::count(drawForms.begin(), drawForms.end(), "рисоваю"), 0);
    EXPECT_TRUE(containsAll(getForms("танцевать"), {"танцую", "танцуешь", "танцевал"}));
    EXPECT_TRUE(containsAll(getForms("воевать"), {"воюю", "воюешь", "воевал"}));
}

TEST(RussianMorphology, FirstSingularAlternation) {
    // Classes 4/5 apply the standard first-person-singular alternations.
    const std::vector<std::string> walkForms = getForms("ходить");
    EXPECT_TRUE(containsAll(walkForms, {"хожу", "ходишь", "ходит", "ходят", "ходил", "ходи"}));
    EXPECT_EQ(std::count(walkForms.begin(), walkForms.end(), "ходю"), 0);
    EXPECT_TRUE(containsAll(getForms("любить"), {"люблю", "любит", "любят", "любил"}));
    EXPECT_TRUE(containsAll(getForms("просить"), {"прошу", "просит", "просил"}));
    EXPECT_TRUE(containsAll(getForms("простить"), {"прощу", "простит", "простил"}));
}

TEST(RussianMorphology, ClassFiveVerbs) {
    // Second conjugation with a non-"и" past tense, from the closed list.
    const std::vector<std::string> seeForms = getForms("видеть");
    EXPECT_TRUE(containsAll(seeForms, {"вижу", "видишь", "видит", "видят", "видел", "видела"}));
    EXPECT_EQ(std::count(seeForms.begin(), seeForms.end(), "видею"), 0);
    EXPECT_TRUE(containsAll(getForms("слышать"), {"слышу", "слышишь", "слышат", "слышал"}));
    EXPECT_TRUE(containsAll(getForms("смотреть"), {"смотрю", "смотрит", "смотрят", "смотрел"}));
    EXPECT_TRUE(containsAll(getForms("стоять"), {"стою", "стоишь", "стоят", "стоял", "стой"}));
    EXPECT_TRUE(containsAll(getForms("спать"), {"сплю", "спишь", "спят", "спал", "спи"}));
}

TEST(RussianMorphology, IrregularNouns) {
    EXPECT_TRUE(containsAll(getForms("мать"), {"мать", "матери", "матерью", "матерей"}));
    EXPECT_TRUE(containsAll(getForms("путь"), {"путь", "пути", "путём", "путям"}));
    EXPECT_TRUE(containsAll(getForms("дочь"), {"дочь", "дочери", "дочерью"}));
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
