load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

# cc_binary(
#     name="solution",
#     srcs=[
#         "solution.cpp",
#     ],
#)

# Targets below were generated from BUCK by scripts/generate_bazel.py.

cc_library(
    name = "tree",
    hdrs = ["tree.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "header",
    hdrs = ["header.h"],
    deps = [
        "//lib:header",
        "//lib:timer",
    ],
    visibility = ["//visibility:public"],
)
