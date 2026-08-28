load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "catboostCompareTest",
    srcs = ["catboostCompareTest.cpp"],
    data = ["catboost_reference.py"],
    deps = [
        "//gtest:gtest",
        "//lib/boosting:trainer",
    ],
)

cc_test(
    name = "boostingTest",
    srcs = ["boostingTest.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/boosting:trainer",
    ],
)
