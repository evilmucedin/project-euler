load("@rules_cc//cc:cc_library.bzl", "cc_library")

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
        "//glog:glog",
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

