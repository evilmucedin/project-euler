#!/usr/bin/env bash

pushd `pwd`
cd "$(dirname "$0")"

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

DIR=$(eulerDir $1)
FNAME=$(eulerFilename $1)
${buck_bin} build @mode/opt ${DIR}/...
res=$?
args="${@:2}"
exe=$(realpath buck-out/gen/${DIR}/${FNAME})

popd
if [ 0 -eq ${res} ]; then
    sudo perf record -g ${exe} ${args}
    sudo perf report
fi
