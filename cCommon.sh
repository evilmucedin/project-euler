#!/usr/bin/env sh

unamestr=`uname`

# Allow override via env var
if [ -n "${BUCK_BIN}" ]; then
    buck_bin="${BUCK_BIN}"
else
    # Prefer buck2 if available, otherwise fall back to legacy buck
    if command -v buck2 >/dev/null 2>&1; then
        buck_bin=`command -v buck2`
    elif command -v buck >/dev/null 2>&1; then
        buck_bin=`command -v buck`
    else
        echo "WARNING: neither buck2 nor buck found in PATH" >&2
        buck_bin=buck2
    fi
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
        echo "${prj##*/}"
    else
        echo "${prj}"
    fi
}
