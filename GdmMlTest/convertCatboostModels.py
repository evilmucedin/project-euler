#!/usr/bin/env python3
import os
import subprocess
import sys

import os
import onnx
from onnxmltools.convert import convert_lightgbm
from skl2onnx.helpers.onnx_helper import save_onnx_model
from onnxmltools.convert.common.data_types import FloatTensorType

import lightgbm as lgb

lgb_model_file = "lightgbm_model.txt"
input_data_file = "data.data"

if not os.path.exists(lgb_model_file):
    raise FileNotFoundError(f"LightGBM model file not found: {lgb_model_file}")
if not os.path.exists(input_data_file):
    raise FileNotFoundError(f"Input data file not found: {input_data_file}")

# Load LightGBM model object
model_lgb = lgb.Booster(model_file=lgb_model_file)

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

# Define ONNX input shape
initial_types = [("input", FloatTensorType([None, n_features]))]
onnx_model_path = "lightgbm_model.onnx"
onnx_model = convert_lightgbm(model_lgb, initial_types=initial_types)
save_onnx_model(onnx_model, onnx_model_path)
print(f"Converted {lgb_model_file} to lightgbm_model.onnx with {n_features} features")

import catboost

onnx_model_path = "lightgbm_model.onnx"
catboostModel = catboost.CatBoost()
catboostModel.load_model(onnx_model_path, format='onnx')
output_path = "lightgbm_model.cbm"
catboostModel.save_model(output_path)
