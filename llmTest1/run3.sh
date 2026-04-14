#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

echo "Building and running calculator3..."

if ! command -v pkg-config >/dev/null 2>&1; then
	echo "Error: pkg-config not found. Install pkg-config and GTK+ development packages." >&2
	exit 1
fi

if ! pkg-config --exists gtk+-3.0; then
	echo "Error: gtk+-3.0 not found by pkg-config. Install GTK+3 dev packages." >&2
	exit 1
fi

CFLAGS=$(pkg-config --cflags gtk+-3.0)
LIBS=$(pkg-config --libs gtk+-3.0)

# Split pkg-config output into an array to avoid word-splitting issues.
# Temporarily restore IFS to include spaces for correct splitting.
OLD_IFS="$IFS"
IFS=$' \t\n'
read -r -a PKG_FLAGS <<< "$(pkg-config --cflags --libs gtk+-3.0)"
IFS="$OLD_IFS"
echo "g++ -std=c++17 calculator3.cpp ${PKG_FLAGS[*]} -o calculator3"
g++ -std=c++17 calculator3.cpp "${PKG_FLAGS[@]}" -o calculator3

echo "Running ./calculator3"
if [ -n "${DISPLAY:-}" ] || [ -n "${WAYLAND_DISPLAY:-}" ]; then
	# Unset potentially harmful env vars that can break symbol lookup (snap, user overrides)
	env -u LD_LIBRARY_PATH -u LD_PRELOAD ./calculator3
else
	if command -v xvfb-run >/dev/null 2>&1; then
		echo "No display detected — running under xvfb-run"
		xvfb-run -s "-screen 0 1024x768x24" env -u LD_LIBRARY_PATH -u LD_PRELOAD ./calculator3
	else
		echo "Error: no X/Wayland display detected and xvfb-run not available." >&2
		echo "Set DISPLAY or install xvfb (package: xvfb) to run the GUI in headless." >&2
		exit 127
	fi
fi

echo "Done."
