load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

# package(default_visibility = ["//ETFModeling"])


cc_binary(
  name="ETFModeling2",
  srcs=[
    "ETFModeling.cpp"
  ],
  deps=[
    "//gflags",
    "//glog:glog2",
    "//lib:header",
    "//lib:init",
    "//lib/thread-pool:thread-pool",
    "//lib:matrix",
    "//lib:random",
    "//lib/stat:stat2",
    "//lib:timer",
    "//lib/ml:dataframe",
  ],
  visibility = ["//ETFModeling:ETFModeling"],
)

#package_group(
#    name = "ETFModeling",
#    packages = [
#    ],
#    includes = [
#        "//ETFModeling:ETFModeling",
#    ],
#)


