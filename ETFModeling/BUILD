# load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
# load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

# package(default_visibility = ["//ETFModeling"])


cc_binary(
    name = "ETFModelingTest",
    srcs = [
        "ETFModelingTest.cpp",
    ],
  deps=[
#    "//gflags:gflags2",
    "//glog:glog2",
    "//lib:header",
    "//lib:init",
    "//lib/thread-pool:thread-pool2",
    "//lib:matrix",
    "//lib:random",
    "//lib/stat:stat2",
    "//lib:timer",
    "//lib:stat",
    "//lib/ml:dataframe",
  ],
#  copts = ["-std=c++17"],
  features = ["-lto"],
)



cc_binary(
  name="ETFModeling2",
  srcs=[
    "ETFModeling.cpp"
  ],
  deps=[
#    "//gflags:gflags2",
    "//glog:glog2",
    "//lib:header",
    "//lib:init",
    "//lib/thread-pool:thread-pool2",
#    "//lib:matrix",
    "//lib:random",
    "//lib/stat:stat2",
    "//lib:timer",
    "//lib:stat",
    "//lib:exception",
    "//lib/ml:dataframe",
  ],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0"],
  features = ["-lto"],
  linkopts = ["-lstdc++"],
#  visibility = ["//ETFModeling:ETFModeling2"],
)
