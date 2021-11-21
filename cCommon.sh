#!/usr/bin/env sh

unamestr=`uname`

buck_bin=buck

if [ "${unamestr}" = "Darwin" ]; then
    buck_bin=`which buck`
fi

eulerDir() {
    prj=$1
    if [[ "${prj}" == *"/"* ]]; then
        echo "${prj}"
    else
        echo "euler${prj}"
    fi
}

eulerFilename() {
    prj=$1
    if [[ "${prj}" == *"/"* ]]; then
        echo "${prj}"
    else
        echo "${prj}"
    fi
}
