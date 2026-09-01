load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "languageData",
    srcs = ["languageData.cpp"],
    hdrs = ["languageData.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "morphology",
    srcs = ["morphology.cpp"],
    hdrs = [
        "morphology.h",
        "utf8.h",
    ],
    visibility = ["//visibility:public"],
    deps = [":languageData"],
)

cc_library(
    name = "embeddingMorphology",
    srcs = ["embeddingMorphology.cpp"],
    hdrs = ["embeddingMorphology.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":languageData",
        ":morphology",
    ],
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
        ":languageData",
        ":morphology",
    ],
)

cc_binary(
    name = "morphologyCompare",
    srcs = ["morphologyCompare.cpp"],
    deps = [
        ":embeddingMorphology",
        ":evaluation",
        ":languageData",
        ":morphology",
    ],
)
