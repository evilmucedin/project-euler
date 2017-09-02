#!/usr/bin/env bash

dir=euler$1
if [ ! -e $dir ]; then
    echo "$dir not found"
    exit 1
fi

pushd .
cd $dir

filename=$1.cpp
if [ -e $filename ]; then
    vim -p $filename BUCK ${@:2}
else
    echo "$filename not found"
    exit 1
fi

popd
