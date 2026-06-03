#!/usr/bin/env bash

set -euo pipefail

pushd "$(pwd)" >/dev/null
cd "$(dirname "$0")"

. ./cCommon.sh

exec "${buck_bin}" build //ETFModeling:ETFModeling
