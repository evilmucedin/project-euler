#!/usr/bin/env bash

. ./cCommon.sh

DR=$(eulerDir $1)
FNAME="$(eulerFilename $1).cpp"
echo "use ${DR} directory and ${FNAME} filename"

if [ ! -d ${DR} ]; then
    echo "${DR} not found"
    exit 1
fi

pushd .
cd ${DR}

echo "Current directory: `pwd`"
if [ -f ${FNAME} ]; then
    vim -p ${FNAME} BUCK ${@:2}
else
    echo "${FMAME} not found"
    exit 1
fi

popd
