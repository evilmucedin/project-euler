#!/usr/bin/env bash

. ./cCommon.sh

set -o pipefail

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

OUT=/tmp/build$1.log
${buck_bin} build @mode/opt euler$1/... 2>&1 | tee ${OUT}
if [[ $? != 0 ]]; then
    vim ${OUT}
fi
