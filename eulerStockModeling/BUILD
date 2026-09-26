# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK.

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "stockModelingLib",
    srcs = [
        "orderBook.cpp",
        "orderBook.h",
    ],
    deps = [
        "//lib:header",
        "//lib:random",
    ],
)

cc_binary(
    name = "StockModeling",
    srcs = [
        "StockModeling.cpp",
        "orderBook.h",
    ],
    deps = [
        ":stockModelingLib",
        "//lib:header",
        "//lib/io:csv",
        "//lib/io:utils",
        "//glog:glog",
    ],
)

cc_test(
    name = "StockModelingTest",
    srcs = [
        "StockModelingTest.cpp",
        "orderBook.h",
    ],
    deps = [
        ":stockModelingLib",
        "//gtest:gtest",
        "//gtest:gtestInt",
        "//lib:header",
    ],
)
