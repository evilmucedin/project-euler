#!/usr/bin/env bash

. ./cCommon.sh

DIR=$(eulerDir $1)
FILENAME=$(eulerFilename $1)
echo "use ${DIR} directory"
if [ ! -d ${DIR} ]; then
    mkdir -p ${DIR}

    cat  >${DIR}/${FILENAME}.cpp << EOF
#include "lib/header.h"

int main() {
    return 0;
}
EOF

    cat  >${DIR}/BUCK << EOF
cxx_binary(
  name="${FILENAME}",
  srcs=[
    "${FILENAME}.cpp"
  ],
  deps=[
    "//lib:header",
  ],
)
EOF

    git add ${DIR}/*
else
    echo ${DIR} exists.
fi
