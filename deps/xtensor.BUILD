load("@rules_cc//cc:defs.bzl", "cc_library")

filegroup(
    name = "all",
    srcs = glob(["include/**"]),
)

cc_library(
    name = "xtensor",
    hdrs = [":all"],
    strip_include_prefix = "include/",
    deps = ["@xtl//:xtl"],
    visibility = ["//visibility:public"],
)
