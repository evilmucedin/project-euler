load("@rules_cc//cc:defs.bzl", "cc_library")

# load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

package(default_visibility = ["//visibility:public"])

# cc_library(
#     name="compression",
#     visibility = ["//compression:public"],
# )

cc_library(
  name="pfordelta",
  hdrs=[
    "pfordelta.h",
  ],
  srcs=[
    "pfordelta.cpp",
  ],
  deps=[
    "//lib:header",
  ],
  features = ["-lto"],
)

