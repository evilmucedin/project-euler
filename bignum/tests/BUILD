load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "bignumTest",
    srcs = ["tests.cpp"],
    deps = [
        "//bignum:bignum",
        "//gtest:gtest",
    ],
)

cc_test(
    name = "piTest",
    srcs = ["pi_test.cpp"],
    deps = [
        "//bignum:bignum",
        "//gtest:gtest",
    ],
)

cc_test(
    name = "cpuCrossCheckTest",
    srcs = ["cpu_crosscheck_test.cpp"],
    deps = [
        "//bignum:bignum",
        "//gtest:gtest",
    ],
)
