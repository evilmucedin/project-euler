# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK.

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "Kalman",
    srcs = ["Kalman.cpp"],
    copts = [
        "-I/opt/intel/compilers_and_libraries_2019.0.117/linux/mkl/include",
        "-I/usr/include/mkl",
    ],
    linkopts = [
        "-L/opt/intel/compilers_and_libraries_2019.0.117/linux/mkl/lib/intel64_lin",
        "-Wl,--no-as-needed",
        "-lmkl_intel_lp64",
        "-lmkl_sequential",
        "-lmkl_core",
        "-lpthread",
        "-lm",
        "-ldl",
    ],
    deps = [
        "//armadillo:armadillo",
        "//lib:header",
        "//lib:random",
        "//glog:glog",
    ],
)
