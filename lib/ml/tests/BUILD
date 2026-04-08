load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "gbdtTest",
    srcs = ["gbdtTest.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/ml:gbdt",
    ],
)

cc_test(
    name = "olsTest",
    srcs = ["olsTest.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/ml:ols",
    ],
)
