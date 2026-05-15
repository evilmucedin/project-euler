#!/usr/bin/env bash
#
# Bootstrap an Ubuntu Linux machine for building this repository with Buck2.
#
# Installs:
#   * APT packages listed in ubuntuPackages.txt
#   * Python (pip) packages listed in ubuntuPipPackages.txt
#   * The Buck2 binary (latest release from facebook/buck2) into ~/.local/bin
#
# Usage:
#   ./setupUbuntu.sh

set -euo pipefail

pushd `pwd` >/dev/null
cd "$(dirname "$0")"

if [ "$(uname)" != "Linux" ]; then
    echo "setupUbuntu.sh is intended for Ubuntu Linux only (uname reports $(uname))." >&2
    exit 1
fi

SUDO=""
if [ "$(id -u)" -ne 0 ]; then
    SUDO=sudo
fi

echo ">>> Installing APT packages from ubuntuPackages.txt"
${SUDO} apt-get update
# shellcheck disable=SC2046
${SUDO} apt-get install -y --no-install-recommends \
    build-essential curl ca-certificates git pkg-config python3 python3-pip \
    $(tr '\n' ' ' < ubuntuPackages.txt)

echo ">>> Installing pip packages from ubuntuPipPackages.txt"
if [ -s ubuntuPipPackages.txt ]; then
    # shellcheck disable=SC2046
    python3 -m pip install --user --upgrade $(tr '\n' ' ' < ubuntuPipPackages.txt)
fi

echo ">>> Installing Buck2"
mkdir -p "${HOME}/.local/bin"
if ! command -v buck2 >/dev/null 2>&1; then
    arch=$(uname -m)
    case "${arch}" in
        x86_64|amd64) buck_arch=x86_64-unknown-linux-musl ;;
        aarch64|arm64) buck_arch=aarch64-unknown-linux-musl ;;
        *) echo "Unsupported architecture: ${arch}" >&2; exit 1 ;;
    esac
    tmp=$(mktemp -d)
    curl -fsSL "https://github.com/facebook/buck2/releases/download/latest/buck2-${buck_arch}.zst" \
        -o "${tmp}/buck2.zst"
    zstd -d "${tmp}/buck2.zst" -o "${HOME}/.local/bin/buck2"
    chmod +x "${HOME}/.local/bin/buck2"
    rm -rf "${tmp}"
    echo "Installed buck2 to ${HOME}/.local/bin/buck2"
    echo "Make sure ${HOME}/.local/bin is on your PATH."
else
    echo "buck2 already installed: $(command -v buck2)"
fi

popd >/dev/null
echo ">>> Done. Try: ./b.sh Normal"
