load("@rules_cc//cc:defs.bzl", "cc_library")
cc_library(
    name="counters",
    hdrs=[
        "counters.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="stat2",
    hdrs=[
        "stat.h",
    ],
    srcs=[
        "stat.cpp",
    ],
    deps=[
        "//lib:header",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="tdigest",
    hdrs=[
        "tdigest.h",
    ],
    srcs=[
        "tdigest.cpp",
    ],
    deps=[
        "//lib:header",
        "//glog:glog",
    ],
    visibility = ["//visibility:public"],
)

# Targets below were generated from BUCK by scripts/generate_bazel.py.

cc_library(
    name = "stat",
    srcs = ["stat.cpp"],
    hdrs = ["stat.h"],
    deps = ["//lib:header"],
    visibility = ["//visibility:public"],
)
