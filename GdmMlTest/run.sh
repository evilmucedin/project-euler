#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++17 -O3 \
  -DNDEBUG -s \
  -I../../LightGBM/include \
  -I/home/denplusplus/Programming/catboost \
  -I/home/denplusplus/Programming/catboost/build_applier/catboost/libs/model_interface \
  -I/home/denplusplus/Programming/onnxruntime/include \
  gdmMlTest.cpp \
  -L../../LightGBM/build_gpp \
  -L/home/denplusplus/Programming/catboost/build_applier/catboost/libs/model_interface \
  -L/home/denplusplus/Programming/catboost \
  -L/home/denplusplus/Programming/onnxruntime/build/Linux/Release \
  -l_lightgbm \
  -lcatboostmodel \
  -lonnxruntime \
  -lstdc++ \
  -o gdmMlTest

export LD_LIBRARY_PATH="../../LightGBM/build_gpp:/home/denplusplus/Programming/catboost/build_applier/catboost/libs/model_interface:/home/denplusplus/Programming/onnxruntime/build/Linux/Release:$LD_LIBRARY_PATH"

# Ensure data exists
python3 generate_data.py

# If CatBoost model not present, train via catboost_run.py if available
if [ -f catboost_run.py ]; then
  echo "Running CatBoost model training/check ..."
  python3 catboost_run.py data.data || echo "Warning: catboost_run.py failed, continuing"
fi

# Run the C++ test binary
./gdmMlTest data.data

# bazel build --sandbox_debug --verbose_failures //GdmMlTest:GdmMlTest --inject_repository=lightgbm_local=/home/denplusplus/Programming/LightGBM

# ./gdmMlTest data.data
# python3 ./run.py

