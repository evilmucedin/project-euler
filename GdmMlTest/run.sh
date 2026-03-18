#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++17 -O2 \
  -I../../LightGBM/include \
  gdmMlTest.cpp \
  -L../../LightGBM/build_gpp \
  -l_lightgbm -lstdc++ \
  -o gdmMlTest

# g++ ... -L../catboost/libs/model_interface -lcatboostmodel

# bazel build --sandbox_debug --verbose_failures //GdmMlTest:GdmMlTest --inject_repository=lightgbm_local=/home/denplusplus/Programming/LightGBM


LD_LIBRARY_PATH=../../LightGBM/build_gpp:$LD_LIBRARY_PATH ./gdmMlTest data.data ./gdmMlTest
