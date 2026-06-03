#!/usr/bin/env bash
#
# Install the latest Buck2 release on macOS.
#
# Usage:
#   ./scripts/installBuck2MacOS.sh
#   PREFIX=/usr/local ./scripts/installBuck2MacOS.sh
#
# Installs buck2 to ${PREFIX:-$HOME/.local}/bin/buck2.

set -euo pipefail

if [ "$(uname)" != "Darwin" ]; then
    echo "installBuck2MacOS.sh is intended for macOS only (uname reports $(uname))." >&2
    exit 1
fi

if ! command -v curl >/dev/null 2>&1; then
    echo "curl is required." >&2
    exit 1
fi

if ! command -v zstd >/dev/null 2>&1; then
    if command -v brew >/dev/null 2>&1; then
        echo ">>> Installing zstd with Homebrew"
        brew install zstd
    else
        echo "zstd is required. Install it with: brew install zstd" >&2
        exit 1
    fi
fi

arch=$(uname -m)
case "${arch}" in
    arm64|aarch64) buck_arch=aarch64-apple-darwin ;;
    x86_64|amd64) buck_arch=x86_64-apple-darwin ;;
    *) echo "Unsupported architecture: ${arch}" >&2; exit 1 ;;
esac

prefix=${PREFIX:-"${HOME}/.local"}
bin_dir="${prefix}/bin"
install_path="${bin_dir}/buck2"

tmp=$(mktemp -d)
trap 'rm -rf "${tmp}"' EXIT

mkdir -p "${bin_dir}"

echo ">>> Downloading Buck2 latest (${buck_arch})"
curl -fsSL "https://github.com/facebook/buck2/releases/download/latest/buck2-${buck_arch}.zst" \
    -o "${tmp}/buck2.zst"
zstd -d -f "${tmp}/buck2.zst" -o "${install_path}"
chmod +x "${install_path}"

echo ">>> Installed Buck2: ${install_path}"
"${install_path}" --version

if [[ ":${PATH}:" != *":${bin_dir}:"* ]]; then
    echo "NOTE: add ${bin_dir} to your PATH to use buck2 without an absolute path."
fi
