# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK.

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "sample1",
    srcs = [
        "sample1.cc",
        "sample1.h",
    ],
)

cc_binary(
    name = "sample1Unittest",
    srcs = [
        "sample1_unittest.cc",
        "sample1.h",
    ],
    deps = [
        ":sample1",
        "//gtest:gtest",
        "//gtest:gtestInt",
    ],
)
