cc_binary(
  name="6",
  srcs=[
    "6.cpp",
  ],
  deps=[
    "//lib:header",
    "//lib:init",
    "//lib:string",
    "//gflags:gflags2",
    "//advent/lib:aoc",
    "//glog2:glog",
#    "@bazel_tools//tools/cpp:gmpxx", # For C++ GMP wrappers
  ],
  linkopts = ["-lgmp"],
)

