load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_import(
    name = "lightgbm",
    shared_library = "/home/denplusplus/Programming/LightGBM/lib_lightgbm.so",
    visibility = ["//visibility:public"],
)

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
    ":lightgbm",
  ],
  copts = ["-I/home/denplusplus/Programming/LightGBM/include", "-std=c++17"],
  features = ["-lto"],
  linkopts = ["-lstdc++"],
#  copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
#  visibility = ["//ETFModeling:ETFModeling2"],
)
