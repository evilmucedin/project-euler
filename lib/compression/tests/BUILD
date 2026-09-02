load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "pfordelta_test",
    srcs = ["pfordelta_test.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/compression:pfordelta",
    ],
)

cc_test(
    name = "lz_test",
    srcs = ["lz_test.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/compression:lz",
    ],
)

cc_test(
    name = "rc_test",
    srcs = ["rc_test.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/compression:lz",
        "//lib/compression:rc",
    ],
)
