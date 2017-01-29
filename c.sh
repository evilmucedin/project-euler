#!/usr/bin/env bash

DIR=euler$1
if [ ! -d ${DIR} ]; then
    mkdir ${DIR}

    cat  >${DIR}/$1.cpp << EOF
#include "lib/header.h"

int main() {
    return 0;
}
EOF

    cat  >${DIR}/BUCK << EOF
cxx_binary(
  name="$1",
  srcs=[
    "$1.cpp"
  ],
  deps=[
    "//lib:header",
  ],
)
EOF

    git add ${DIR}/*

fi
