#!/usr/bin/env python3
import os
import subprocess
import sys

import os

import lightgbm as lgb

lgb_model_file = "lightgbm_model.txt"
input_data_file = "data.data"

if not os.path.exists(lgb_model_file):
    raise FileNotFoundError(f"LightGBM model file not found: {lgb_model_file}")
if not os.path.exists(input_data_file):
    raise FileNotFoundError(f"Input data file not found: {input_data_file}")

# Infer number of features from first non-empty line in data.data
n_features = None
with open(input_data_file, 'r') as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        parts = [x for x in line.split(',') if x != '']
        if not parts:
            continue
        # last column is label
        n_features = len(parts) - 1
        break

if n_features is None or n_features <= 0:
    raise ValueError(f"Unable to determine number of features from {input_data_file}")

