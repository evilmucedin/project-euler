#!/usr/bin/env sh

. ./cCommon.sh

measure_time=0
while getopts "t" opt; do
    case "$opt" in
    t) 
        measure_time=1
        ;;
    esac
done

${buck_bin} build @mode/opt euler$1/...
res=$?
if [ 0 -eq ${res} ]; then
    if [ ${measure_time} ]; then
       time buck-out/gen/euler$1/$1
    else
       buck-out/gen/euler$1/$1
    fi
fi
