#!/usr/bin/env bash
set -e

echo "Starting script..."
# Use pkg-config to get the correct include and link flags for gtk+-3.0
cmd='g++ calculator3.cpp $(pkg-config --cflags --libs gtk+-3.0) -o calculator3'
echo "Cmd we run: $cmd"
eval $cmd
./calculator3
echo "Script completed."
