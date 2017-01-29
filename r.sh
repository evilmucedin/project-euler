#!/usr/bin/env sh

unamestr=`uname`

buck_bin=buck

if [ "${unamestr}" = "Darwin" ]; then
    buck_bin=~/Temp/buck/bin/buck
fi

${buck_bin} build @mode/opt euler$1/...
res=$?
if [ 0 -eq ${res} ]; then
    buck-out/gen/euler$1/$1
fi
