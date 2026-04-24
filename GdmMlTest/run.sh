#!/usr/bin/env bash
set -euo pipefail

<<<<<<< HEAD
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Allow overriding dependency roots from the environment.
LIGHTGBM_DIR="${LIGHTGBM_DIR:-/home/denplusplus/Programming/LightGBM}"
CATBOOST_DIR="${CATBOOST_DIR:-/home/denplusplus/Programming/catboost}"
ONNXRUNTIME_DIR="${ONNXRUNTIME_DIR:-/home/denplusplus/Programming/onnxruntime}"
TL2CGEN_DIR="/home/denplusplus/Programming/tl2cgen/include}"

LIGHTGBM_INCLUDE="$LIGHTGBM_DIR/include"
LIGHTGBM_LIB="$LIGHTGBM_DIR/build_gpp"
CATBOOST_INCLUDE="$CATBOOST_DIR"
CATBOOST_MODEL_INTERFACE_LIB="$CATBOOST_DIR/build_applier/catboost/libs/model_interface"
ONNX_INCLUDE="$ONNXRUNTIME_DIR/include"
ONNX_LIB="$ONNXRUNTIME_DIR/build/Linux/Release"
TL2CGEN_LIB="$ONNXRUNTIME_DIR/build/Linux/Release"

missing=()
[[ -d "$LIGHTGBM_INCLUDE" ]] || missing+=("LightGBM include dir: $LIGHTGBM_INCLUDE")
[[ -d "$LIGHTGBM_LIB" ]] || missing+=("LightGBM lib dir: $LIGHTGBM_LIB")
[[ -d "$CATBOOST_INCLUDE" ]] || missing+=("CatBoost include dir: $CATBOOST_INCLUDE")
[[ -d "$CATBOOST_MODEL_INTERFACE_LIB" ]] || missing+=("CatBoost model_interface lib dir: $CATBOOST_MODEL_INTERFACE_LIB")
[[ -d "$ONNX_INCLUDE" ]] || missing+=("ONNX Runtime include dir: $ONNX_INCLUDE")
[[ -d "$ONNX_LIB" ]] || missing+=("ONNX Runtime lib dir: $ONNX_LIB")

if (( ${#missing[@]} > 0 )); then
  printf 'Missing dependencies for gdmMlTest build:\n' >&2
  for p in "${missing[@]}"; do
    printf '  - %s\n' "$p" >&2
  done
  cat >&2 <<EOF

Set dependency paths and rerun, for example:
  LIGHTGBM_DIR=/path/to/LightGBM \
  CATBOOST_DIR=/path/to/catboost \
  ONNXRUNTIME_DIR=/path/to/onnxruntime \
  ./run.sh
EOF
  exit 1
fi

=======
<<<<<<< HEAD
>>>>>>> ee168571e320dce3eafc6655b284fad8ae780efb
g++ -std=c++17 -O3 \
  -DNDEBUG -s \
  -I"$LIGHTGBM_INCLUDE" \
  -I"$CATBOOST_INCLUDE" \
  -I"$CATBOOST_MODEL_INTERFACE_LIB" \
  -I"$ONNX_INCLUDE" \
  -I"TL2CGEN_INCLUDE" \
  gdmMlTest.cpp \
<<<<<<< HEAD
=======
  -L../../LightGBM/build_gpp \
  -L/home/denplusplus/Programming/catboost/build_applier/catboost/libs/model_interface \
  -L/home/denplusplus/Programming/catboost \
  -L/home/denplusplus/Programming/onnxruntime/build/Linux/Release \
=======
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Allow overriding dependency roots from the environment.
LIGHTGBM_DIR="${LIGHTGBM_DIR:-/home/denplusplus/Programming/LightGBM}"
CATBOOST_DIR="${CATBOOST_DIR:-/home/denplusplus/Programming/catboost}"
ONNXRUNTIME_DIR="${ONNXRUNTIME_DIR:-/home/denplusplus/Programming/onnxruntime}"
TL2CGEN_DIR="/home/denplusplus/Programming/tl2cgen/include}"

LIGHTGBM_INCLUDE="$LIGHTGBM_DIR/include"
LIGHTGBM_LIB="$LIGHTGBM_DIR/build_gpp"
CATBOOST_INCLUDE="$CATBOOST_DIR"
CATBOOST_MODEL_INTERFACE_LIB="$CATBOOST_DIR/build_applier/catboost/libs/model_interface"
ONNX_INCLUDE="$ONNXRUNTIME_DIR/include"
ONNX_LIB="$ONNXRUNTIME_DIR/build/Linux/Release"
TL2CGEN_LIB="$ONNXRUNTIME_DIR/build/Linux/Release"

missing=()
[[ -d "$LIGHTGBM_INCLUDE" ]] || missing+=("LightGBM include dir: $LIGHTGBM_INCLUDE")
[[ -d "$LIGHTGBM_LIB" ]] || missing+=("LightGBM lib dir: $LIGHTGBM_LIB")
[[ -d "$CATBOOST_INCLUDE" ]] || missing+=("CatBoost include dir: $CATBOOST_INCLUDE")
[[ -d "$CATBOOST_MODEL_INTERFACE_LIB" ]] || missing+=("CatBoost model_interface lib dir: $CATBOOST_MODEL_INTERFACE_LIB")
[[ -d "$ONNX_INCLUDE" ]] || missing+=("ONNX Runtime include dir: $ONNX_INCLUDE")
[[ -d "$ONNX_LIB" ]] || missing+=("ONNX Runtime lib dir: $ONNX_LIB")

if (( ${#missing[@]} > 0 )); then
  printf 'Missing dependencies for gdmMlTest build:\n' >&2
  for p in "${missing[@]}"; do
    printf '  - %s\n' "$p" >&2
  done
  cat >&2 <<EOF

Set dependency paths and rerun, for example:
  LIGHTGBM_DIR=/path/to/LightGBM \
  CATBOOST_DIR=/path/to/catboost \
  ONNXRUNTIME_DIR=/path/to/onnxruntime \
  ./run.sh
EOF
  exit 1
fi

g++ -std=c++17 -O3 \
  -DNDEBUG -s \
  -I"$LIGHTGBM_INCLUDE" \
  -I"$CATBOOST_INCLUDE" \
  -I"$CATBOOST_MODEL_INTERFACE_LIB" \
  -I"$ONNX_INCLUDE" \
  -I"TL2CGEN_INCLUDE" \
  gdmMlTest.cpp \
>>>>>>> ee168571e320dce3eafc6655b284fad8ae780efb
  -L"$LIGHTGBM_LIB" \
  -L"$CATBOOST_MODEL_INTERFACE_LIB" \
  -L"$CATBOOST_DIR" \
  -L"$ONNX_LIB" \
  -L"$TL2CGEN_LIB" \
<<<<<<< HEAD
=======
>>>>>>> 5dd2c62abcc99357697d23886080f074089f813f
>>>>>>> ee168571e320dce3eafc6655b284fad8ae780efb
  -l_lightgbm \
  -lcatboostmodel \
  -lonnxruntime \
  -lstdc++ \
  -o gdmMlTest

<<<<<<< HEAD
export LD_LIBRARY_PATH="$LIGHTGBM_LIB:$CATBOOST_MODEL_INTERFACE_LIB:$ONNX_LIB:${LD_LIBRARY_PATH:-}"
=======
<<<<<<< HEAD
export LD_LIBRARY_PATH="../../LightGBM/build_gpp:/home/denplusplus/Programming/catboost/build_applier/catboost/libs/model_interface:/home/denplusplus/Programming/onnxruntime/build/Linux/Release:$LD_LIBRARY_PATH"
=======
export LD_LIBRARY_PATH="$LIGHTGBM_LIB:$CATBOOST_MODEL_INTERFACE_LIB:$ONNX_LIB:${LD_LIBRARY_PATH:-}"
>>>>>>> 5dd2c62abcc99357697d23886080f074089f813f
>>>>>>> ee168571e320dce3eafc6655b284fad8ae780efb

./gdmMlTest data.data

# bazel build --sandbox_debug --verbose_failures //GdmMlTest:GdmMlTest --inject_repository=lightgbm_local=/home/denplusplus/Programming/LightGBM

# ./gdmMlTest data.data
# python3 ./run.py

