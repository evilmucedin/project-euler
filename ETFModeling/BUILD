load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

package(default_visibility = ["//ETFModeling"])

cc_binary(
  name="ETFModeling",
  srcs=[
    "ETFModeling.cpp"
  ],
  deps=[
#    "//gflags:gflags",
    "//glog:glog",
    "//lib:header",
    "//lib:init",
    "//lib/thread-pool:thread-pool",
    "//lib:matrix",
    "//lib:random",
    "//lib:stat",
    "//lib:timer",
    "//lib/ml:dataframe",
  ],
)
