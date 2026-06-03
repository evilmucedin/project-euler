#!/usr/bin/env bash
#
# Install a known-good Buck2 release on Ubuntu Linux.
#
# Usage:
#   ./scripts/installBuck2Ubuntu.sh
#   PREFIX=/usr/local ./scripts/installBuck2Ubuntu.sh
#
# Installs buck2 to ${PREFIX:-$HOME/.local}/bin/buck2.
# Override the release with BUCK2_VERSION=YYYY-MM-DD if needed.

set -euo pipefail

if [ "$(uname)" != "Linux" ]; then
    echo "installBuck2Ubuntu.sh is intended for Ubuntu Linux only (uname reports $(uname))." >&2
    exit 1
fi

if ! command -v apt-get >/dev/null 2>&1; then
    echo "installBuck2Ubuntu.sh requires apt-get and is intended for Ubuntu/Debian systems." >&2
    exit 1
fi

SUDO=()
if [ "$(id -u)" -ne 0 ]; then
    SUDO=(sudo)
fi

echo ">>> Installing Buck2 download dependencies"
"${SUDO[@]}" apt-get update
"${SUDO[@]}" apt-get install -y --no-install-recommends ca-certificates curl zstd

arch=$(uname -m)
case "${arch}" in
    x86_64|amd64) buck_arch=x86_64-unknown-linux-musl ;;
    aarch64|arm64) buck_arch=aarch64-unknown-linux-musl ;;
    *) echo "Unsupported architecture: ${arch}" >&2; exit 1 ;;
esac

prefix=${PREFIX:-"${HOME}/.local"}
bin_dir="${prefix}/bin"
install_path="${bin_dir}/buck2"

tmp=$(mktemp -d)
trap 'rm -rf "${tmp}"' EXIT

mkdir -p "${bin_dir}"

# Do not use the historical `latest` tag: it points at an old prerelease whose
# bundled prelude does not parse with current Buck2 projects on Ubuntu 26.04
# (for example, `prelude//apple/apple_rules_impl.bzl` can fail on @oss-enable
# guarded blocks). Pin a dated release so the binary and bundled prelude stay
# in sync and CI/developer machines are reproducible.
buck2_version=${BUCK2_VERSION:-2026-06-01}

echo ">>> Downloading Buck2 ${buck2_version} (${buck_arch})"
curl -fsSL "https://github.com/facebook/buck2/releases/download/${buck2_version}/buck2-${buck_arch}.zst" \
    -o "${tmp}/buck2.zst"
zstd -d -f "${tmp}/buck2.zst" -o "${install_path}"
chmod +x "${install_path}"

echo ">>> Installed Buck2: ${install_path}"
"${install_path}" --version

if [[ ":${PATH}:" != *":${bin_dir}:"* ]]; then
    echo "NOTE: add ${bin_dir} to your PATH to use buck2 without an absolute path."
fi
