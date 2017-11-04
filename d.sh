#!/usr/bin/env bash

. ./cCommon.sh

measure_time=0
while getopts "t" opt; do
    case ${opt} in
    \? )
        echo Unknown option
        ;;
    : )
        echo Missing argument
        ;;
    * )
        echo Unimplemented
        ;;
    esac
done

shift $((${OPTIND} - 1))

${buck_bin} build @mode/dbg euler$1/...
res=$?
if [ 0 -eq ${res} ]; then
    gdb buck-out/gen/euler$1/$1
fi
