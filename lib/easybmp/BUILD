load("@rules_cc//cc:defs.bzl", "cc_library")
cc_library(
    name="easybmp2",
    srcs=[
        "EasyBMP.cpp",
    ],
    hdrs=[
        "EasyBMP.h",
        "EasyBMP_BMP.h",
        "EasyBMP_DataStructures.h",
        "EasyBMP_VariousBMPutilities.h",
    ],
    includes=["."],
    # copts=["-stb=c++23"],
    visibility = ["//visibility:public"],
)

# Targets below were generated from BUCK by scripts/generate_bazel.py.

cc_library(
    name = "easybmp",
    srcs = ["EasyBMP.cpp"],
    hdrs = [
        "EasyBMP.h",
        "EasyBMP_BMP.h",
        "EasyBMP_DataStructures.h",
        "EasyBMP_VariousBMPutilities.h",
    ],
    visibility = ["//visibility:public"],
)
