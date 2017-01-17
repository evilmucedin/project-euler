#!/usr/bin/env sh

buck build @mode/opt euler$1/...
buck-out/gen/euler$1/$1
