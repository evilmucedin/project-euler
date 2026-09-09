#!/usr/bin/env bash
# Build and run the graphs library tests and examples on Ubuntu.
# Installs gfortran and make via apt when they are missing.
set -euo pipefail
cd "$(dirname "$0")"

if ! command -v gfortran >/dev/null 2>&1 || ! command -v make >/dev/null 2>&1; then
  echo "gfortran/make not found; installing via apt..."
  SUDO=""
  if [ "$(id -u)" -ne 0 ]; then
    SUDO="sudo"
  fi
  $SUDO apt-get update
  $SUDO apt-get install -y gfortran make
fi

make clean
make test
make run
echo "Ubuntu: tests and examples completed successfully"
