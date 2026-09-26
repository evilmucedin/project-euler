load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
  name="calculusTest",
  size="small",
  srcs=[
    "calculusTest.cpp",
  ],
  deps=[
    "//lib/calculus:calculus",
    "//gtest:gtest",
  ],
  features = ["-lto"],
)
