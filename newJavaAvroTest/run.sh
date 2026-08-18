#!/bin/sh
# Build (if needed) and run the Avro teaching app, forwarding all arguments.
# Runs the built binary directly (not 'bazel run') so that relative file
# paths on the command line resolve against the caller's directory.
#
# Examples:
#   ./run.sh create people.avro
#   ./run.sh add people.avro Katherine 44 Hampton
#   ./run.sh dump people.avro
#   ./run.sh ask people.avro average-age
set -e
caller_dir=$(pwd)
cd "$(dirname "$0")"
bazel build //newJavaAvroTest:AvroTour >/dev/null 2>&1 || bazel build //newJavaAvroTest:AvroTour
binary=$(bazel info bazel-bin)/newJavaAvroTest/AvroTour
cd "$caller_dir"
exec "$binary" "$@"
