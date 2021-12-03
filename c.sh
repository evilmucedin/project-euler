#!/usr/bin/env bash

. ./cCommon.sh

DIR=$(eulerDir $1)
FILENAME=$(eulerFilename $1)
echo "use ${DIR} directory and ${FILENAME} filename"

if [ ! -d ${DIR} ]; then
    mkdir -p ${DIR}

    cat  >${DIR}/${FILENAME}.cpp << EOF
#include "lib/header.h"
#include "lib/init.h"
#include "glog/logging.h"

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
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
    "//lib:init",
    "//glog:glog",
  ],
)
EOF

    git add ${DIR}/*
else
    echo ${DIR} exists.
fi
