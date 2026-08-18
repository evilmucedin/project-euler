#!/bin/sh
# Compile the Avro teaching app with Bazel.
set -e
cd "$(dirname "$0")"
bazel build //newJavaAvroTest:AvroTour
echo "compiled: run with './run.sh <command> ...'"
