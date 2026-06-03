#!/usr/bin/env sh

unamestr=`uname`

# Buck is retired for this repository; require Buck2. Allow override via env var.
if [ -n "${BUCK_BIN:-}" ]; then
    buck_bin="${BUCK_BIN}"
elif [ -x "${HOME}/.local/bin/buck2" ]; then
    # setupUbuntu.sh installs the repository-pinned Buck2 here. Prefer it over
    # an older system buck2 on PATH because stale binaries can fail while
    # parsing their bundled prelude on Ubuntu 26.04.
    buck_bin="${HOME}/.local/bin/buck2"
elif command -v buck2 >/dev/null 2>&1; then
    buck_bin=`command -v buck2`
else
    echo "ERROR: buck2 not found. Install Buck2, add ~/.local/bin to PATH, or set BUCK_BIN=/path/to/buck2." >&2
    exit 1
fi
if [ "`basename "${buck_bin}"`" = "buck" ]; then
    echo "ERROR: legacy buck is retired; use buck2 instead." >&2
    exit 1
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
