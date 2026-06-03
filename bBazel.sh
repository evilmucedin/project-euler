#!/usr/bin/env bash

set -euo pipefail

pushd "$(pwd)" >/dev/null
cd "$(dirname "$0")"

BAZEL_BIN=${BAZEL_BIN:-}
if [ -z "${BAZEL_BIN}" ]; then
    if command -v bazel >/dev/null 2>&1; then
        BAZEL_BIN=$(command -v bazel)
    elif command -v bazelisk >/dev/null 2>&1; then
        BAZEL_BIN=$(command -v bazelisk)
    else
        echo "ERROR: neither bazel nor bazelisk was found in PATH. Install Bazel or set BAZEL_BIN=/path/to/bazel." >&2
        exit 1
    fi
fi

if [ "$#" -eq 0 ]; then
    cat >&2 <<'USAGE'
Usage:
  ./bBazel.sh <bazel target> [extra bazel args...]

Examples:
  ./bBazel.sh //advent/2024/1:all
  ./bBazel.sh //ETFModeling:ETFModeling2

Bazel metadata exists for a subset of this repository. Prefer Buck2 for the
main build and Ninja for CI smoke builds.
USAGE
    exit 2
fi

exec "${BAZEL_BIN}" build "$@"
