#!/usr/bin/env sh
g++ solution2.cpp -o solution2 -I/opt/homebrew/include -L/opt/homebrew/lib -lmpfr -lgmp
clang++ solution2.cpp -o solution2 -I/opt/homebrew/include -L/opt/homebrew/lib -lmpfr -lgmp
./solution2
