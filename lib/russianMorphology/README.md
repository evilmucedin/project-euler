# lib/russianMorphology

A small, dictionary-free Russian morphology library built on A. A. Zaliznyak's
classification system («Грамматический словарь русского языка»). Given a
Russian word in UTF-8, it infers the word's Zaliznyak inflection class and
generates the inflected forms of that same word from the paradigm tables of
that class.

Two interchangeable classifiers are provided on top of the shared paradigm
tables:

- a **rule-based** backend (`morphology.h`) that reads the class off the word
  shape with hand-written rules and exception lists, and
- an **embedding** backend (`embeddingMorphology.h`) that predicts the class
  with a fastText-style subword-embedding classifier trained in-process on a
  small labeled lexicon — fully local, deterministic, no files or network.

All the Russian language data both backends share — the paradigm ending
tables, the closed exception lists, the irregular paradigms, and the labeled
training lexicon — lives in one dependency-free library
(`languageData.h`/`languageData.cpp`), which changes rarely; the backends
contain only classification and generation logic.

On a held-out gold set the embedding backend classifies **89.3%** of words
into the correct inflection class vs **65.0%** for the rules (see
[Quality comparison](#quality-comparison)).

## Rule-based approach

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

## Embedding approach

The embedding backend replaces the hand-written classification rules with a
learned classifier while reusing the same paradigm tables for generation
(`analyzeWithClass()`):

- Every hashed character n-gram (n = 2..5 of the boundary-padded word
  `<слово>`, plus the whole word) owns a 48-dimensional vector; the word
  embedding is their average — the fastText subword scheme, so the model
  generalizes to unseen words through their substrings.
- A softmax layer over the embedding predicts the Zaliznyak class. N-gram
  vectors and softmax weights are trained jointly by SGD on the ~1100-word
  labeled lexicon embedded in `languageData.cpp`.
- Training runs on first use, in-process, in ~0.1 s, and is deterministic
  (fixed-seed PRNG, fixed shuffle order): no model files, downloads, or
  network access, matching the library's dictionary-free spirit.
- Prediction is **shape-constrained**: only classes whose paradigm is
  compatible with the word shape (`classCompatible()`) compete in the argmax.
  The classifier therefore spends its capacity on the genuinely ambiguous
  decisions — `сеть`/`читать` (noun vs infinitive), `соль`/`руль` (feminine
  vs masculine `-ь`), `гений`/`синий` (noun vs adjective `-ий`),
  `кино`/`окно` (indeclinable vs neuter), `слышать`/`слушать` (second vs
  first conjugation).

Both backends consult the same short built-in list of fully irregular
paradigms (`мать`, `дочь`, `путь`) before classifying.

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

The embedding backend mirrors the same entry points and adds access to the
embeddings themselves:

```cpp
#include "lib/russianMorphology/embeddingMorphology.h"

namespace emb = russianMorphology::embedding;

// Drop-in equivalents of analyze()/getForms() driven by the classifier.
russianMorphology::TAnalysis analysis = emb::analyze("гавань");
// analysis.zaliznyakIndex == "ж 8"  (the rules misread гавань as masculine)

// The predicted class with its confidence among shape-compatible classes.
emb::TPrediction prediction = emb::classify("гавань");

// The 48-dim L2-normalized word vector, and cosine similarity.
std::vector<float> vec = emb::wordEmbedding("соль");
float sim = emb::cosineSimilarity(vec, emb::wordEmbedding("честность"));
```

## Quality comparison

`evaluation.h` defines a 103-word gold set with the correct class, required
forms, and characteristic wrong forms per word. The set is disjoint from the
embedding backend's training lexicon (a test enforces this), so it measures
generalization to unseen words; it deliberately concentrates on the ambiguous
shapes listed above, alongside regular vocabulary. Measured by
`morphologyCompare` (reproducible — training is deterministic):

| backend | part of speech | inflection class | paradigm correct* |
|---|---|---|---|
| rules | 93/103 (90.3%) | 67/103 (65.0%) | 67/103 (65.0%) |
| embeddings | 99/103 (96.1%) | 92/103 (89.3%) | 92/103 (89.3%) |

*paradigm correct = all required forms generated and no forbidden form.

Where the difference comes from — the rules fail systematically on whole
shape classes, the embeddings on individual hard words:

- Feminine soft-sign nouns outside the rules' exception list (`гавань`,
  `скатерть`, `ладонь`, `фасоль`, ...) are all declined masculine by the
  rules; the classifier learns the feminine-leaning stem patterns.
- Nouns in `-ий`/`-ой` (`калий`, `конвой`, `прибой`) are all claimed by the
  rules' adjective heuristic.
- Indeclinable loanwords in `-о`/`-е` (`манто`, `кабаре`, `фойе`) are all
  declined as neuters by the rules.
- Second-conjugation verbs in `-ать`/`-еть` outside the rules' closed class-5
  list (`дрожать`, `жужжать`, `сипеть`) get first-conjugation forms.
- The embeddings' remaining losses are individually ambiguous words:
  `нить`/`рать`/`сеть`/`кровать` (nouns with infinitive shape),
  `пудель`/`миндаль`/`хрусталь` (masculine soft-sign nouns misread feminine),
  `копыто`/`корыто`/`табло` (declinable vs indeclinable `-о`), and `толстеть`
  (`гл 1` vs `гл 5` in `-стеть`).

```sh
ninja lib/russianMorphology/morphologyCompare
./build-ninja/bin/lib/russianMorphology/morphologyCompare --verbose
```

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
# --embedding switches to the embedding backend
./build-ninja/bin/lib/russianMorphology/morphologyCli --embedding гавань
# with no arguments the CLI reads words from stdin, one per line
```

## Tests

The gtest suite builds and runs through the Ninja build (`cxx_test` targets
are not wired to a Buck2 test toolchain in this repo):

```sh
python3 scripts/generate_ninja.py   # only needed after BUCK/BUILD changes
ninja lib/russianMorphology/tests/morphologyTest \
      lib/russianMorphology/tests/embeddingMorphologyTest \
      lib/russianMorphology/tests/comparisonTest
./build-ninja/bin/lib/russianMorphology/tests/morphologyTest
./build-ninja/bin/lib/russianMorphology/tests/embeddingMorphologyTest
./build-ninja/bin/lib/russianMorphology/tests/comparisonTest
```

`morphologyTest` covers the rule-based backend, `embeddingMorphologyTest` the
embedding model (determinism, vector shape, classification, paradigms,
training-set accuracy), and `comparisonTest` locks in the measured quality of
both backends on the gold set, including train/eval disjointness and the
requirement that the embeddings beat the rules on held-out words.
