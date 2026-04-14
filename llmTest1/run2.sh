#!/usr/bin/env bash
set -e

echo "Starting script..."
# Use pkg-config to get the correct include and link flags for gtk+-3.0
cmd='g++ calculator2.cpp $(pkg-config --cflags --libs gtk+-3.0) -o calculator2'
echo "Cmd we run: $cmd"
eval $cmd
./calculator2
echo "Script completed."
