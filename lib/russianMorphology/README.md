# lib/russianMorphology

A small, dictionary-free Russian morphology library. Given a Russian word in
UTF-8, it guesses the word's paradigm from its ending and generates the
inflected forms of that same word.

## API

```cpp
#include "lib/russianMorphology/morphology.h"

// All morphological forms of the word (lowercase UTF-8, deduplicated,
// normalized input first).
std::vector<std::string> forms = russianMorphology::getForms("книга");
// -> книга книги книге книгу книгой книг книгам книгами книгах

// Same, plus the guessed part of speech.
russianMorphology::TAnalysis analysis = russianMorphology::analyze("читать");
// analysis.partOfSpeech == EPartOfSpeech::Verb
```

What is generated:

- **Nouns** — case/number forms for the standard declensions: 1st declension
  (`-а`/`-я`/`-ия`), 2nd declension masculine (hard consonant, `-й`, `-ий`,
  `-ь`) and neuter (`-о`/`-е`/`-ие`), 3rd declension feminine (`-ь`).
- **Adjectives** (`-ый`/`-ий`/`-ой`) — gender, number, and case agreement
  forms for hard, soft, velar, and hushing stems.
- **Verbs** (infinitives in vowel + `-ть`) — present tense (all six persons),
  past tense (all genders and plural), and imperative, using the productive
  first/second conjugation patterns.

Russian spelling rules are applied at the stem/ending seam (`и` instead of
`ы` after velars and hushings, `а`/`у` instead of `я`/`ю` after hushings and
`ц`), so `книга -> книги`, `лечить -> лечу/лечат`, `хороший -> хорошая`.

A few common irregular words (`мать`, `дочь`, `путь`) are built in, plus a
short exception list of nouns whose endings look like infinitives (`сеть`,
`кровать`, ...). Input that is not a single Russian word is returned
unchanged with `EPartOfSpeech::Unknown`.

## Limitations

This is a rule-based guesser, not a dictionary-backed analyzer:

- Part of speech and gender are guessed from the ending, so ambiguous words
  can get the wrong paradigm (e.g. the noun "гений" declines as an
  adjective would, "дверь" is guessed masculine unless listed).
- No stress information: variants that depend on stress (`ножом` vs `мужем`,
  `конём` vs `конем`) pick one common spelling.
- No fleeting vowels or consonant mutations (`окно -> окон`,
  `писать -> пишу` are not produced); the productive paradigm is used
  instead, as it would be for a neologism.
- Verb aspect, participles, and gerunds are not generated.

## CLI

```sh
buck2 run //lib/russianMorphology:morphologyCli -- книга читать хороший
# or via Ninja:
ninja lib/russianMorphology/morphologyCli
./build-ninja/bin/lib/russianMorphology/morphologyCli книга читать хороший
# with no arguments the CLI reads words from stdin, one per line
```

## Tests

The gtest suite builds and runs through the Ninja build (`cxx_test` targets
are not wired to a Buck2 test toolchain in this repo):

```sh
python3 scripts/generate_ninja.py   # only needed after BUCK/BUILD changes
ninja lib/russianMorphology/tests/morphologyTest
./build-ninja/bin/lib/russianMorphology/tests/morphologyTest
```
