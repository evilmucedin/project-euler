#!/usr/bin/env bash
#
# Bootstrap an Ubuntu Linux machine for building this repository.
#
# Installs:
#   * Bootstrap build tools needed by Buck2 and Ninja
#   * APT packages listed in ubuntuPackages.txt
#   * Python packages listed in ubuntuPipPackages.txt and pipPackages.txt
#   * The Buck2 binary (latest release from facebook/buck2) into ~/.local/bin
#
# Usage:
#   ./setupUbuntu.sh

set -euo pipefail

cd "$(dirname "$0")"

if [ "$(uname)" != "Linux" ]; then
    echo "setupUbuntu.sh is intended for Ubuntu Linux only (uname reports $(uname))." >&2
    exit 1
fi

if ! command -v apt-get >/dev/null 2>&1; then
    echo "setupUbuntu.sh requires apt-get and is intended for Ubuntu/Debian systems." >&2
    exit 1
fi

SUDO=()
if [ "$(id -u)" -ne 0 ]; then
    SUDO=(sudo)
fi

read_word_file() {
    local file="$1"
    [ -f "${file}" ] || return 0
    # Allow whitespace-separated words and optional comments in package files.
    sed 's/#.*//' "${file}" | tr '[:space:]' '\n' | awk 'NF && !seen[$0]++'
}

dedupe_words() {
    awk 'NF && !seen[$0]++'
}

APT_BOOTSTRAP_PACKAGES=(
    ca-certificates
    curl
    git
    python3
    python3-pip
    zstd
)

APT_BUILD_PACKAGES=(
    build-essential
    clang
    lld
    ninja-build
    pkg-config
)

mapfile -t APT_FILE_PACKAGES < <(read_word_file ubuntuPackages.txt)
mapfile -t APT_PACKAGES < <(printf '%s\n' "${APT_BOOTSTRAP_PACKAGES[@]}" "${APT_BUILD_PACKAGES[@]}" "${APT_FILE_PACKAGES[@]}" | dedupe_words)

if [ "${#APT_PACKAGES[@]}" -gt 0 ]; then
    echo ">>> Installing Ubuntu APT packages"
    "${SUDO[@]}" apt-get update
    "${SUDO[@]}" apt-get install -y --no-install-recommends "${APT_PACKAGES[@]}"
fi

mapfile -t PIP_PACKAGES < <(
    {
        read_word_file ubuntuPipPackages.txt
        read_word_file pipPackages.txt
    } | dedupe_words
)

if [ "${#PIP_PACKAGES[@]}" -gt 0 ]; then
    echo ">>> Installing Python packages"
    if ! python3 -m pip install --user --upgrade --break-system-packages "${PIP_PACKAGES[@]}"; then
        python3 -m pip install --user --upgrade "${PIP_PACKAGES[@]}"
    fi
fi

echo ">>> Installing Buck2"
if command -v buck2 >/dev/null 2>&1; then
    echo "buck2 already installed: $(command -v buck2)"
else
    ./scripts/installBuck2Ubuntu.sh
fi

if [[ ":${PATH}:" != *":${HOME}/.local/bin:"* ]]; then
    echo "NOTE: add ${HOME}/.local/bin to your PATH to use installed user tools."
fi

echo ">>> Done. Try:"
echo "    ./b.sh Normal"
echo "    python3 scripts/generate_ninja.py && ninja advent/2020/1/1"
