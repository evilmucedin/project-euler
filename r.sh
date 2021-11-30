#!/usr/bin/env bash

. ./cCommon.sh

measure_time=0
while getopts "t" opt; do
    case ${opt} in
    t )
        measure_time=1
        ;;
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

DIR=$(eulerDir $1)
FNAME=$(eulerFilename $1)
${buck_bin} build @mode/opt ${DIR}/...
res=$?
args="${@:2}"
if [ 0 -eq ${res} ]; then
    if [ ${measure_time} -eq 1 ]; then
       time buck-out/gen/${DIR}/${FNAME} $args
    else
       buck-out/gen/${DIR}/${FNAME} $args
    fi
fi
