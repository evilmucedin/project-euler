load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name="grid",
    hdrs=[
        "grid.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="field",
    hdrs=[
        "field.h",
    ],
    srcs=[
        "field.cpp",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="boundary",
    hdrs=[
        "boundary.h",
    ],
    srcs=[
        "boundary.cpp",
    ],
    deps=[
        ":field",
        ":grid",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="poisson",
    hdrs=[
        "poisson.h",
    ],
    srcs=[
        "poisson.cpp",
    ],
    deps=[
        ":boundary",
        ":field",
        ":grid",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="diagnostics",
    hdrs=[
        "diagnostics.h",
    ],
    srcs=[
        "diagnostics.cpp",
    ],
    deps=[
        ":field",
        ":grid",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="obstacle",
    hdrs=[
        "obstacle.h",
    ],
    srcs=[
        "obstacle.cpp",
    ],
    deps=[
        ":field",
        ":grid",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="io",
    hdrs=[
        "io.h",
    ],
    srcs=[
        "io.cpp",
    ],
    deps=[
        ":field",
        ":grid",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="solver",
    hdrs=[
        "solver.h",
    ],
    srcs=[
        "solver.cpp",
    ],
    deps=[
        ":boundary",
        ":field",
        ":grid",
        ":poisson",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="navierStokes",
    hdrs=[
        "navierStokes.h",
    ],
    deps=[
        ":boundary",
        ":diagnostics",
        ":field",
        ":grid",
        ":io",
        ":obstacle",
        ":poisson",
        ":solver",
    ],
    visibility = ["//visibility:public"],
)
