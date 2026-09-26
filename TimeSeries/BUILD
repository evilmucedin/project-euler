# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK.

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "TimeSeries",
    srcs = [
        "DNN.cpp",
        "TimeSeries.cpp",
        "DNN.h",
    ],
    deps = [
        "//armadillo:armadillo",
        "//eigen:eigen",
        "//gflags:gflags",
        "//lib:datetime",
        "//lib:fft",
        "//lib:init",
        "//lib/io:csv",
        "//lib/io:stream",
        "//lib/io:utils",
        "//lib/io:zstream",
        "//lib:math",
        "//lib:matrix",
        "//lib:progressBar",
        "//lib:random",
        "//lib:string",
        "//lib:timer",
        "//tiny_dnn:tiny_dnn",
        "//lib:header",
    ],
)
