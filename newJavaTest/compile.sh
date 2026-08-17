#!/bin/sh
# Compile test.java. Requires JDK 21+ (sealed patterns, virtual threads).
set -e
cd "$(dirname "$0")"
javac test.java
echo "compiled: run with 'java Tour'"
