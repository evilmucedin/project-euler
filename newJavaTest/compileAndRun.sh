#!/bin/sh
# Compile test.java via compile.sh, then run the resulting Tour class.
set -e
cd "$(dirname "$0")"
./compile.sh
java Tour
