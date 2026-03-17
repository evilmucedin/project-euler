#!/usr/bin/env python3
"""Generate data.data with 100+ features for ML training and testing."""
import os
import random

NUM_FEATURES = 100
NUM_ROWS = 800
SEED = 42
DATA_PATH = os.path.join(os.path.dirname(__file__), "data.data")

def main():
    random.seed(SEED)
    with open(DATA_PATH, "w") as f:
        for i in range(NUM_ROWS):
            # 100 features: mix of deterministic and random, label from threshold on first 20
            row = []
            s = 0.0
            for j in range(NUM_FEATURES):
                x = (i / NUM_ROWS) * (1 + 0.1 * (j % 10)) + 0.1 * random.random()
                row.append(round(x, 6))
                if j < 20:
                    s += x
            label = 1 if s > 10.0 else 0
            f.write(",".join(str(v) for v in row) + f",{label}\n")
    print(f"Wrote {DATA_PATH}: {NUM_ROWS} rows, {NUM_FEATURES} features")

if __name__ == "__main__":
    main()

