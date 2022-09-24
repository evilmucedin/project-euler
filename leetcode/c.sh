#!/usr/bin/env bash

mkdir $1
    
    cat  >$1/$1.cpp << EOF
#include "../header.h"

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol. << endl;

    return 0;
}
EOF

    cat  >$1/BUCK << EOF
cxx_binary(
  name="${1}",
  srcs=[
    "${1}.cpp"
  ],
  deps=[
    "//lib:header",
    "//lib:init",
  ],
)
EOF

git add ${1}/*
