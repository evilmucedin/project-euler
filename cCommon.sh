#!/usr/bin/env sh

unamestr=`uname`

buck_bin=buck

if [ "${unamestr}" = "Darwin" ]; then
    buck_bin=`which buck`
fi


