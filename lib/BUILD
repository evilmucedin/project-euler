# load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
# load("@rules_cc//cc:cc_library.bzl", "cc_library")

# package(default_visibility = ["//lib"])


# load("@pybind11_bazel//:build_defs.bzl", "pybind_library")

cc_library(
    name="lib",
    deps=[
        ":header",
        ":exception",
        ":string",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="noncopyable",
    srcs=[
        "noncopyable.cpp",
    ],
    hdrs=[
        "noncopyable.h",
    ],
    visibility = ["//visibility:public"],
)

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
        "//gflags:gflags2",
        "//glog:glog2",
    ],
    visibility=[
	    "//visibility:public",
    ],
)


cc_library(
    name="file",
    hdrs=[
        "file.h",
    ],
    srcs=[
        "file.cpp",
    ],
    deps=[
        ":header",
        ":string",
    ],
    visibility=[
        "//visibility:public",
    ],
#    copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
#    alwayslink = 1,
)

cc_library(
    name="string",
    hdrs=[
        "string.h",
    ],
    srcs=[
        "string.cpp",
    ],
    deps=[
        ":header",
        ":exception",
    ],
    visibility=[
	    "//visibility:public",
    ],
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
#    alwayslink = 1,
)


cc_library(
    name = "matrix",
    hdrs = ["matrix.h"],
    srcs = ["matrix.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
#    alwayslink = 1,
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
)

cc_library(
    name = "random",
    hdrs = ["random.h"],
    srcs = ["random.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
#    alwayslink = 1,
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
)

cc_library(
    name = "timer",
    hdrs = ["timer.h"],
    srcs = ["timer.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
#    alwayslink = 1,
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
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
#    alwayslink = 1,
)

cc_library(
    name="stat",
    hdrs=[
        "stat.h",
    ],
    deps=[
        ":header",
    ],
    visibility = ["//visibility:public"],
    alwayslink = 1,
)

