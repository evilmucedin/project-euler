#!/usr/bin/env python3
"""Reference runner for lib/boosting comparison tests.

Trains an official CatBoost model on CSV data produced by
catboostCompareTest.cpp with hyperparameters matched to lib/boosting, and
prints machine-readable `key=value` metrics: training time, prediction time,
and test quality. Kept dependency-minimal: numpy and catboost only.
"""

import argparse
import sys
import time


def read_csv(path):
    features = []
    targets = []
    with open(path, "r", encoding="utf-8") as fd:
        for line in fd:
            parts = line.strip().split(",")
            if not parts or parts == [""]:
                continue
            features.append([float(x) for x in parts[:-1]])
            targets.append(float(parts[-1]))
    return features, targets


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--train-csv", required=True)
    parser.add_argument("--test-csv", required=True)
    parser.add_argument("--loss", choices=["RMSE", "Logloss"], required=True)
    parser.add_argument("--iterations", type=int, required=True)
    parser.add_argument("--depth", type=int, required=True)
    parser.add_argument("--learning-rate", type=float, required=True)
    parser.add_argument("--l2-leaf-reg", type=float, required=True)
    parser.add_argument("--border-count", type=int, required=True)
    parser.add_argument("--seed", type=int, default=42)
    args = parser.parse_args()

    import numpy as np
    from catboost import CatBoostClassifier, CatBoostRegressor

    train_x, train_y = read_csv(args.train_csv)
    test_x, test_y = read_csv(args.test_csv)
    train_x = np.array(train_x, dtype=np.float32)
    train_y = np.array(train_y, dtype=np.float32)
    test_x = np.array(test_x, dtype=np.float32)
    test_y = np.array(test_y, dtype=np.float32)

    # Matched to lib/boosting: plain boosting, symmetric trees, no bootstrap,
    # single thread. CatBoost's remaining advantages (better border selection,
    # leaf estimation iterations, ...) are part of what we measure.
    params = dict(
        iterations=args.iterations,
        depth=args.depth,
        learning_rate=args.learning_rate,
        l2_leaf_reg=args.l2_leaf_reg,
        border_count=args.border_count,
        loss_function=args.loss,
        boosting_type="Plain",
        bootstrap_type="No",
        random_seed=args.seed,
        thread_count=1,
        allow_writing_files=False,
        verbose=False,
    )
    if args.loss == "RMSE":
        model = CatBoostRegressor(**params)
    else:
        model = CatBoostClassifier(**params)

    start = time.perf_counter()
    model.fit(train_x, train_y)
    train_time_ms = (time.perf_counter() - start) * 1000.0

    start = time.perf_counter()
    if args.loss == "RMSE":
        predictions = model.predict(test_x)
    else:
        predictions = model.predict_proba(test_x)[:, 1]
    predict_time_ms = (time.perf_counter() - start) * 1000.0

    print(f"train_time_ms={train_time_ms:.3f}")
    print(f"predict_time_ms={predict_time_ms:.3f}")
    if args.loss == "RMSE":
        mse = float(np.mean((predictions - test_y) ** 2))
        print(f"test_mse={mse:.8f}")
    else:
        accuracy = float(np.mean((predictions > 0.5) == (test_y > 0.5)))
        eps = 1e-15
        clipped = np.clip(predictions, eps, 1.0 - eps)
        logloss = float(-np.mean(test_y * np.log(clipped) + (1.0 - test_y) * np.log(1.0 - clipped)))
        print(f"test_accuracy={accuracy:.6f}")
        print(f"test_logloss={logloss:.8f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
