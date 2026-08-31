load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "morphology",
    srcs = ["morphology.cpp"],
    hdrs = ["morphology.h"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "morphologyCli",
    srcs = ["main.cpp"],
    deps = [":morphology"],
)
