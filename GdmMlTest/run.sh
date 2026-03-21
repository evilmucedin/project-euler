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

./gdmMlTest data.data

# bazel build --sandbox_debug --verbose_failures //GdmMlTest:GdmMlTest --inject_repository=lightgbm_local=/home/denplusplus/Programming/LightGBM

# ./gdmMlTest data.data
# python3 ./run.py

