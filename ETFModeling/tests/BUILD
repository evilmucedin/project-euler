load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
  name="etfModelTest",
  size="small",
  srcs=[
    "etfModelTest.cpp",
  ],
  deps=[
    "//ETFModeling:etfModel",
    "//gflags:gflags2",
    "//lib:header",
    "//lib:exception",
    "//lib:stat",
    "//lib/ml:dataframe",
    "//gtest:gtest",
  ],
  data=[
    "//ETFModeling:marketData/SPY.csv",
  ],
  features = ["-lto"],
)
