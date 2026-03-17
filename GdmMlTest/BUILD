load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
   name="GdmMlTest",
   srcs=[
     "gdmMlTest.cpp",
   ],
   deps=[
    "//gflags:gflags2",
    "//glog2:glog",
    "//lib:header",
    "//lib:init",
    "//lib/thread-pool:thread-pool2",
#    "//lib:matrix",
    "//lib:random",
#    "//lib/stat:stat2",
    "//lib:timer",
    "//lib:stat",
#    "//lib:exception",
    "//lib/ml:dataframe",
#    "//lib/io:csv",
#    "//lib:file",
    "@lightgbm_local//:lightgbm",
  ],
  copts = ["-std=c++17"],
  features = ["-lto"],
  linkopts = [
    "-lstdc++",
  ],
#  visibility = ["//ETFModeling:ETFModeling2"],
)
