# lib/boosting

Gradient boosting on oblivious decision trees, following the core design
principles of the CatBoost library:

- **Oblivious (symmetric) trees.** Every node on a tree level tests the same
  `feature > border` condition, so a depth-`D` tree is just `D` splits plus
  `2^D` leaf values, and the leaf of a sample is the `D`-bit mask of its split
  outcomes. This makes serving extremely cheap: `D` float comparisons and one
  array lookup per tree, no pointers, no allocation.
- **Feature quantization.** Before training, every feature is binarized into
  at most `borderCount` bins over quantile borders; all split search runs on
  compact `uint8` bins with histogram accumulation.
- **Second-order leaf estimates.** Leaves are Newton steps
  `-G / (H + l2LeafReg)` from the per-leaf gradient and hessian sums, with L2
  leaf regularization (CatBoost's `l2_leaf_reg`).
- **Losses.** `RMSE` for regression and `LogLoss` for binary classification
  (targets in {0, 1}); classification predictions come back through a sigmoid
  as probabilities.

## Training and saving a model

```cpp
#include "lib/boosting/trainer.h"

boosting::TDataset dataset(/*featureCount=*/4);
dataset.addRow({0.1f, 0.2f, 0.3f, 0.4f}, /*target=*/1.5f);
// ... more rows ...

boosting::TTrainParams params;
params.iterations = 200;
params.depth = 6;
params.learningRate = 0.1;
params.loss = boosting::ELoss::RMSE;

boosting::TModel model = boosting::TTrainer(params).train(dataset);
model.save("model.cbm");
```

## Realtime application of a saved model

```cpp
#include "lib/boosting/model.h"

const boosting::TModel model = boosting::TModel::load("model.cbm");

float features[4] = {0.1f, 0.2f, 0.3f, 0.4f};
double score = model.apply(features);            // raw score
double p = model.applyProbability(features);     // for LogLoss models
```

The model file is a small versioned binary format (magic `CBM1`); `load`
validates the header and structure and throws `std::runtime_error` on
corrupted input.

## Layout

- `model.h/.cpp` — serving-side model: oblivious trees, apply, save/load.
- `dataset.h/.cpp` — training data container and quantile binarization.
- `trainer.h/.cpp` — histogram-based level-wise boosting trainer.
- `tests/` — gtest coverage: leaf indexing, regression and classification
  quality, subsampling determinism, save/load roundtrips.

## Comparison against the official CatBoost library

`tests/catboostCompareTest.cpp` trains this library and official CatBoost on
identical synthetic datasets with matched hyperparameters (200 symmetric trees
of depth 6, plain boosting, no bootstrap, single thread) and compares test-set
quality and speed. CatBoost is driven through `tests/catboost_reference.py`;
the tests skip when `python3` with the `catboost` package is unavailable
(override the interpreter with `CATBOOST_PYTHON`, the script location with
`CATBOOST_REFERENCE_SCRIPT`; run from the repo root).

Sample run (Apple M-series, CatBoost 1.2.10, single thread):

| metric | lib/boosting | CatBoost |
| --- | --- | --- |
| regression test MSE | 0.00327 | 0.00304 |
| classification test accuracy | 0.951 | 0.951 |
| classification test logloss | 0.1933 | 0.1943 |
| train time, regression (6000 rows) | 52 ms | 184 ms |
| apply, per row (realtime path) | 0.38 us | — (batch only) |

The asserted bounds are intentionally loose (quality within a small factor of
CatBoost's); the printed `[ compare ]` lines carry the exact numbers.

Not implemented (relative to real CatBoost): ordered boosting, categorical
feature target statistics, GPU training, and multiclass losses.
