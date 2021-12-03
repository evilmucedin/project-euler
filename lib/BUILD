load("@pybind11_bazel//:build_defs.bzl", "pybind_library")

pybind_library(
    name = "header",
    hdrs = ["header.h"],
    visibility = ["//visibility:public"],
)

pybind_library(
    name = "matrix",
    hdrs = ["matrix.h"],
    srcs = ["matrix.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
)

pybind_library(
    name = "random",
    hdrs = ["random.h"],
    srcs = ["random.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
)

pybind_library(
    name = "timer",
    hdrs = ["timer.h"],
    srcs = ["timer.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
)

