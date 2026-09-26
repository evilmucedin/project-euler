# load("//toolchain:cc_toolchain")

load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])


cc_binary(
    name = "gflagsTest",
    srcs = ["test.cpp", "test.h"],
    deps = [
        "//gflags:gflags",
        # "//gflags2:gflags",
        # "@com_github_gflags_gflags//:gflags",
        # Other dependencies
    ],
)
