#!/usr/bin/env bash

filename=euler$1/$1.cpp
if [ -e $filename ]; then
    vim -p $filename euler$1/BUCK ${@:2}
else
    echo "$filename not found"
    exit 1
fi
