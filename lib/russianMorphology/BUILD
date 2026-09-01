load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "morphology",
    srcs = ["morphology.cpp"],
    hdrs = [
        "morphology.h",
        "utf8.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "embeddingMorphology",
    srcs = ["embeddingMorphology.cpp"],
    hdrs = ["embeddingMorphology.h"],
    visibility = ["//visibility:public"],
    deps = [":morphology"],
)

cc_library(
    name = "evaluation",
    srcs = ["evaluation.cpp"],
    hdrs = ["evaluation.h"],
    visibility = ["//visibility:public"],
    deps = [":morphology"],
)

cc_binary(
    name = "morphologyCli",
    srcs = ["morphologyCLI.cpp"],
    deps = [
        ":embeddingMorphology",
        ":morphology",
    ],
)

cc_binary(
    name = "morphologyCompare",
    srcs = ["morphologyCompare.cpp"],
    deps = [
        ":embeddingMorphology",
        ":evaluation",
        ":morphology",
    ],
)
