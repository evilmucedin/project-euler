load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
  name="yfinanace",
  hdrs=[
    "base.h",
    "benchmark.h",
    "methods.h",
    "requests.h",
    "structures.h",
    "utils.h",
  ],
  srcs=[
    "base.cpp",
    "methods.cpp",
    "requests.cpp",
    "utils.cpp",
  ],
  deps=[
    "//lib:header",
#    "//lib/cpr:cpr2",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
)
