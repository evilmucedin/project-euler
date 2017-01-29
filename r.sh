#!/usr/bin/env sh

buck build @mode/opt euler$1/... 
res=$?
if [ 0 -eq ${res} ]; then
    buck-out/gen/euler$1/$1
fi
