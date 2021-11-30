#!/usr/bin/env sh

. ../cCommon.sh

set -e
set -x

project=$(pwd)
project="${project#*euler/}"
echo $project
${buck_bin} build @mode/opt //$project/...


