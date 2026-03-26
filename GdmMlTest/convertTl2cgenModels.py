#!/usr/bin/env python3
import os
import subprocess
import sys

import os

import treelite
import tl2cgen

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

# Load LightGBM model object
model_lgb = lgb.Booster(model_file=lgb_model_file)

# 2. Load the LightGBM model into Treelite
# Option A: From an in-memory Booster object
model = treelite.frontend.from_lightgbm(model_lgb)

# Option B: From a saved model file
# bst.save_model("lightgbm_model.txt")
# model = treelite.Model.load("lightgbmTl2cgenMmodel.txt", model_format="lightgbm")

# 3. Generate C code or Export a Shared Library using TL2cgen
# Exporting as a shared library (.so or .dll)
tl2cgen.export_lib(model, toolchain="gcc", libpath="./predictor.so")

# 2. Export the model as a native shared library
# You can choose "gcc", "clang", or "msvc" as the toolchain
tl2cgen.export_lib(
    model, 
    toolchain="gcc", 
    libpath="./modelTl2cgenPredictor.so", 
    params={"parallel_comp": 4}  # Optional: use 4 threads for compilation
)


# Alternatively, generate raw C source code package
# tl2cgen.export_srcpkg(model, toolchain="gcc", pkgpath="./model_pkg.zip", libname="model")

print("Conversion successful! Model exported to ./predictor.so")

# 4. (Optional) Run prediction using the generated library
predictor = tl2cgen.Predictor("./predictor.so")
# dmat = tl2cgen.DMatrix(data)
# predictions = predictor.predict(dmat)
