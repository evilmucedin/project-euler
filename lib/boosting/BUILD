load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "model",
    srcs = ["model.cpp"],
    hdrs = ["model.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "dataset",
    srcs = ["dataset.cpp"],
    hdrs = ["dataset.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "trainer",
    srcs = ["trainer.cpp"],
    hdrs = ["trainer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":dataset",
        ":model",
    ],
)
