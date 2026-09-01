load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "morphologyTest",
    srcs = ["morphologyTest.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/russianMorphology:morphology",
    ],
)

cc_test(
    name = "embeddingMorphologyTest",
    srcs = ["embeddingMorphologyTest.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/russianMorphology:embeddingMorphology",
        "//lib/russianMorphology:languageData",
        "//lib/russianMorphology:morphology",
    ],
)

cc_test(
    name = "comparisonTest",
    srcs = ["comparisonTest.cpp"],
    deps = [
        "//gtest:gtest",
        "//lib/russianMorphology:embeddingMorphology",
        "//lib/russianMorphology:evaluation",
        "//lib/russianMorphology:languageData",
        "//lib/russianMorphology:morphology",
    ],
)
