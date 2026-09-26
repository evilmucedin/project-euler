# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK.

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "MatrixTest",
    srcs = ["MatrixTest.cpp"],
    linkopts = [
        "-lmkl_intel_thread",
        "-lmkl_gf_lp64",
        "-lmkl_core",
        "-lmkl_intel_lp64",
        "-lmkl_intel_thread",
        "-lmkl_core",
        "-lmkl_def",
        "-llapack",
        "-lblas",
        "-lomp",
    ],
    deps = [
        "//armadillo:armadillo",
        "//lib:header",
        "//lib:random",
        "//lib:timer",
        "//gflags:gflags",
    ],
)
