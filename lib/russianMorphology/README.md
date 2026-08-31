# lib/russianMorphology

A small, dictionary-free Russian morphology library built on A. A. Zaliznyak's
classification system («Грамматический словарь русского языка»). Given a
Russian word in UTF-8, it infers the word's Zaliznyak inflection class from
its shape and generates the inflected forms of that same word from the
paradigm tables of that class.

## Approach

Instead of stripping suffixes (stemming), every recognized word is mapped to
an inflection class, and the forms come from an explicit ending table for
that class, mirroring the tables in the appendix of Zaliznyak's dictionary:

- **Nouns** — gender + stem type 1–8: 1 hard consonant, 2 soft consonant,
  3 velar (`г/к/х`), 4 hushing (`ж/ч/ш/щ`), 5 `ц`, 6 vowel/`ь` stem
  (`герой`, `шея`, `платье`), 7 `и`-stem (`гений`, `армия`, `здание`),
  8 third declension (`ночь`, `путь`). Each (gender, type) pair has its own
  table of case/number endings, so the spelling rules (`книги`, not `книгы`;
  `ножей`, not `ножов`) are part of the paradigm rather than a post-fixup.
- **Adjectives** — stem types 1–4 with the stress scheme read off the
  nominative ending: scheme a for `-ый`/`-ий`, scheme b for `-ой`
  (`красный` п 1a, `русский` п 3a, `хороший` п 4a, `большой` п 4b).
- **Verbs** — the productive Zaliznyak conjugation classes inferable from
  the infinitive: 1 (`читать` → `читаю`), 2 (`рисовать` → `рисую`,
  `воевать` → `воюю`), 3 (`тянуть` → `тяну`), 4 (`говорить` → `говорю`),
  plus a closed list of class-5 verbs (`слышать`, `видеть`, `стоять`, ...).
  The standard first-person-singular alternations of classes 4/5 are applied:
  `ходить` → `хожу`, `любить` → `люблю`, `простить` → `прощу`.

The inferred class is reported in `TAnalysis::zaliznyakIndex`: `"м 1"`,
`"ж 8"`, `"с 7"` for nouns, `"п 4b"` for adjectives, `"гл 4"` for verbs, and
`"0"` for indeclinables (Zaliznyak's notation for them).

## API

```cpp
#include "lib/russianMorphology/morphology.h"

// All morphological forms of the word (lowercase UTF-8, deduplicated,
// normalized input first).
std::vector<std::string> forms = russianMorphology::getForms("книга");
// -> книга книги книге книгу книгой книг книгам книгами книгах

// Same, plus the inferred part of speech and Zaliznyak index.
russianMorphology::TAnalysis analysis = russianMorphology::analyze("читать");
// analysis.partOfSpeech == EPartOfSpeech::Verb
// analysis.zaliznyakIndex == "гл 1"
```

A few fully irregular words (`мать`, `дочь`, `путь`) are built in, plus a
short exception list of feminine soft-sign nouns whose endings look like
infinitives or masculines (`сеть`, `кровать`, `дверь`, ...). Input that is
not a single Russian word is returned unchanged with
`EPartOfSpeech::Unknown`.

## Limitations

The classification is inferred from the word shape, not looked up in the
dictionary:

- Part of speech and gender are guessed from the ending, so ambiguous words
  can get the wrong class (`гений` matches the adjective shape `-ий`).
- Stress schemes (Zaliznyak's a–f) are not recoverable from the shape alone.
  Where the spelling of an ending depends on stress (`ножом` vs `маршем`,
  `отцов` vs `месяцев`), the tables generate both variants; `ё`-variants are
  written with `е`.
- Fleeting vowels (Zaliznyak's `*`: `окно` → `окон`, `отец` → `отца`) are
  not modeled; the plain paradigm is used, as it would be for a neologism.
- Only the productive verb classes plus the class-5 list are covered;
  primary irregular verbs (`жить`, `пить`, `брать`, `писать`) get
  productive-class forms. Verb aspect, participles, and gerunds are not
  generated.

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
