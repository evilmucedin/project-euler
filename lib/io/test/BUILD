# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK.

load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "zstreamTest",
    srcs = ["zstreamTest.cpp"],
    deps = [
        "//lib:header",
        "//lib:random",
        "//lib/io:utils",
        "//lib/io:zstream",
        "//glog:glog",
        "//gtest:gtest",
        "//gtest:gtestInt",
        "//lib/io:stream",
    ],
)

cc_test(
    name = "streamTest",
    srcs = ["streamTest.cpp"],
    deps = [
        "//glog:glog",
        "//lib:header",
        "//lib/io:stream",
        "//lib:random",
        "//lib:timer",
        "//gtest:gtest",
        "//gtest:gtestInt",
    ],
)

cc_test(
    name = "csvTest",
    srcs = ["csvTest.cpp"],
    deps = [
        "//lib:header",
        "//lib/io:csv",
        "//gtest:gtest",
        "//gtest:gtestInt",
        "//lib/io:stream",
    ],
)

cc_test(
    name = "binarySerializationTest",
    srcs = ["binarySerializationTest.cpp"],
    deps = [
        "//lib:random",
        "//lib/io:binarySerialization",
        "//lib/io:zstream",
        "//gtest:gtest",
        "//gtest:gtestInt",
        "//lib:header",
        "//lib/io:stream",
    ],
)
