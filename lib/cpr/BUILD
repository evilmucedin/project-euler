load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
  name="cpr",
  hdrs=[
    "api.h",
    "async.h",
    "async_wrapper.h",
    "cpr.h",
    "singleton.h",
  ],
  srcs=[
  ],
  deps=[
    "//lib:header",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
)

