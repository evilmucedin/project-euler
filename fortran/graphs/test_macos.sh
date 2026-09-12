#!/usr/bin/env bash
# Build and run the graphs library tests and examples on macOS.
# Installs gfortran via Homebrew's gcc package when it is missing.
set -euo pipefail
cd "$(dirname "$0")"

if ! command -v gfortran >/dev/null 2>&1; then
  if command -v brew >/dev/null 2>&1; then
    echo "gfortran not found; installing gcc via Homebrew..."
    brew install gcc
  else
    echo "error: gfortran not found and Homebrew is not installed." >&2
    echo "Install Homebrew (https://brew.sh) and run: brew install gcc" >&2
    exit 1
  fi
fi

make clean
make test
make run
echo "macOS: tests and examples completed successfully"
