#!/usr/bin/env bash
# Regenerate and open the percolation graphs:
#   percolation.png      - conductivity and average current density vs. metal fraction
#   percolationField.png - material grid, potential, and current density of single samples
#
# Usage: ./showGraphs.sh [--regen]
#   --regen  rerun the simulations even if the CSV data already exists

set -euo pipefail
cd "$(dirname "$0")"

BIN=../build-ninja/bin/percolationModel/percolationModel
if [ ! -x "${BIN}" ]; then
    echo "Building percolationModel..." >&2
    (cd .. && ninja percolationModel/percolationModel)
fi

# Pick a Python interpreter that has matplotlib.
PY=""
for candidate in python3 /usr/bin/python3; do
    if "${candidate}" -c "import matplotlib" >/dev/null 2>&1; then
        PY="${candidate}"
        break
    fi
done
if [ -z "${PY}" ]; then
    echo "ERROR: no python3 with matplotlib found. Try: pip3 install matplotlib" >&2
    exit 1
fi

REGEN=0
if [ "${1:-}" = "--regen" ]; then
    REGEN=1
fi

if [ "${REGEN}" = 1 ] || [ ! -f conductivity.csv ]; then
    echo "Running conductivity sweep (~15s)..." >&2
    "${BIN}"
fi
"${PY}" draw.py

for pp in 0.50 0.59 0.70; do
    prefix="field_p${pp#0.}"
    if [ "${REGEN}" = 1 ] || [ ! -f "${prefix}_grid.csv" ]; then
        "${BIN}" field 48 "${pp}" 1 "${prefix}"
    fi
done
"${PY}" drawField.py field_p50 field_p59 field_p70

case "$(uname)" in
    Darwin) OPEN=open ;;
    *) OPEN=xdg-open ;;
esac
"${OPEN}" percolation.png
"${OPEN}" percolationField.png
