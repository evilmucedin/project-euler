# load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
# load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

# package(default_visibility = ["//ETFModeling"])


package_group(
    name = "ETFModeling3",
    packages = [
    ],
    includes = [
        "//ETFModeling:ETFModeling2",
    ],
)



cc_binary(
  name="ETFModeling2",
  srcs=[
    "ETFModeling.cpp"
  ],
  deps=[
    "//gflags:gflags2",
    "//glog:glog2",
    "//lib:header",
    "//lib:init",
    "//lib/thread-pool:thread-pool2",
    "//lib:matrix",
    "//lib:random",
    "//lib/stat:stat2",
    "//lib:stat",
    "//lib:timer",
    "//lib/ml:dataframe",
  ],
#  visibility = ["//ETFModeling:ETFModeling2"],
)
