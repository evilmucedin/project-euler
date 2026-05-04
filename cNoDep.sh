#!/usr/bin/env bash

pushd `pwd`
cd "$(dirname "$0")"

. ./cCommon.sh

DIR=$(eulerDir $1)
FILENAME=$(eulerFilename $1)
echo "use ${DIR} directory and ${FILENAME} filename"

if [ ! -d ${DIR} ]; then
    mkdir -p ${DIR}

    cat  >${DIR}/${FILENAME}.cpp << EOF
#include <cstdio>

#include <iostream>
#include <vector>

using namespace std;

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
  ],
)
EOF

    git add ${DIR}/*
else
    echo ${DIR} exists.
fi
