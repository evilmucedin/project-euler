<<<<<<< HEAD
load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

package(default_visibility = ["//lib"])


# load("@pybind11_bazel//:build_defs.bzl", "pybind_library")
=======
load("@rules_cc//cc:cc_library.bzl", "cc_library")
>>>>>>> d2a0517c27499ea265c39e58af870378d920f64a

cc_library(
    name = "header",
    hdrs = ["header.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name="init",
    hdrs=[
        "init.h",
    ],
    srcs=[
        "init.cpp",
    ],
    deps=[
        ":header",
        "//gflags:gflags",
        "//glog:glog2",
    ],
    visibility=[
	    "//visibility:public",
    ],
)


cc_library(
    name = "matrix",
    hdrs = ["matrix.h"],
    srcs = ["matrix.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
)

cc_library(
    name = "random",
    hdrs = ["random.h"],
    srcs = ["random.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
)

cc_library(
    name = "timer",
    hdrs = ["timer.h"],
    srcs = ["timer.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
)

cc_library(
    name="exception",
    srcs=[
        "exception.cpp",
    ],
    hdrs=[
        "exception.h",
    ],
    deps=[
        ":header",
    ],
    visibility = ["//visibility:public"],
)

