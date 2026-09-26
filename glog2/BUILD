load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

licenses(["notice"])

exports_files(["COPYING"])

# load("@rules_cc//cc:defs.bzl", "cc_binary")
load(":bazel/glog.bzl", "glog_library")

# generate_bazel: skip (glog_library() below defines :glog)
# Built with gflags, like the BUCK target, so that programs using both
# libraries share one set of FLAGS_*.
glog_library(with_gflags=1, gflags_dep="//gflags:gflags2", gtest_dep="//gtest:gtest")

# platform() to build with clang-cl on Bazel CI. This is enabled with
# the flags in .bazelci/presubmit.yml:
#
#   --incompatible_enable_cc_toolchain_resolution
#   --extra_toolchains=@local_config_cc//:cc-toolchain-x64_windows-clang-cl
#   --extra_execution_platforms=//:x64_windows-clang-cl
platform(
    name = "x64_windows-clang-cl",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
        "@rules_cc//cc/private/toolchain:clang-cl",
    ],
)
