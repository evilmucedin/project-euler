load("@rules_cc//cc:defs.bzl", "cc_binary")
cc_binary(
    name = "eulerCovMatrix",
    srcs = [
        "CovMatrix.cpp",
    ],
    deps = [
        #"@com_github_google_glog//:glog",
        "//glog2:glog",
        "//lib:header",
        "//lib:matrix",
        "//lib:random",
        "//lib:timer",
    ]
)

# Targets below were generated from BUCK by scripts/generate_bazel.py.

cc_binary(
    name = "CovMatrix",
    srcs = ["CovMatrix.cpp"],
    deps = [
        "//glog:glog",
        "//lib:header",
        "//lib:matrix",
        "//lib:random",
        "//lib:timer",
    ],
)
