load("@rules_cc//cc:defs.bzl", "cc_library")

# load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

# package(default_visibility = ["//lib/ml"])

cc_library(
    name="ml",
    visibility = ["//visibility:public"],
)

cc_library(
  name="regressor",
  hdrs=[
    "regressor.h",
  ],
  srcs=[
    "regressor.cpp",
  ],
  deps=[
    "//lib:header",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
)

cc_library(
  name="metrics",
  hdrs=[
    "metrics.h",
  ],
  srcs=[
    "metrics.cpp",
  ],
  deps=[
    "//lib:header",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
)

cc_library(
  name="gbdt",
  hdrs=[
    "gbdt.h",
  ],
  srcs=[
    "gbdt.cpp",
  ],
  deps=[
    "//lib:header",
    "//lib:exception",
    "//lib:noncopyable",
    "//lib:string",
    ":regressor",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
)

cc_library(
  name="ols",
  hdrs=[
    "ols.h",
  ],
  srcs=[
    "ols.cpp",
  ],
  deps=[
    "//lib:header",
    ":regressor",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
)

cc_library(
  name="dataframe",
  hdrs=[
    "dataframe.h",
  ],
  srcs=[
    "dataframe.cpp",
  ],
  deps=[
    "//lib:header",
    "//lib:string",
    "//lib/io:csv",
    "//lib:file",
    "//lib:exception",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
  alwayslink=1,
)

