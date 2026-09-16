load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name="fieldTest",
    srcs=[
        "fieldTest.cpp",
    ],
    deps=[
        "//lib/NavierStokesEquations:field",
        "//gtest:gtest",
    ],
)

cc_test(
    name="boundaryTest",
    srcs=[
        "boundaryTest.cpp",
    ],
    deps=[
        "//lib/NavierStokesEquations:boundary",
        "//lib/NavierStokesEquations:field",
        "//lib/NavierStokesEquations:grid",
        "//lib/NavierStokesEquations:poisson",
        "//gtest:gtest",
    ],
)

cc_test(
    name="poissonTest",
    srcs=[
        "poissonTest.cpp",
    ],
    deps=[
        "//lib/NavierStokesEquations:boundary",
        "//lib/NavierStokesEquations:field",
        "//lib/NavierStokesEquations:grid",
        "//lib/NavierStokesEquations:poisson",
        "//gtest:gtest",
    ],
)

cc_test(
    name="solverTest",
    srcs=[
        "solverTest.cpp",
    ],
    deps=[
        "//lib/NavierStokesEquations:boundary",
        "//lib/NavierStokesEquations:diagnostics",
        "//lib/NavierStokesEquations:field",
        "//lib/NavierStokesEquations:grid",
        "//lib/NavierStokesEquations:obstacle",
        "//lib/NavierStokesEquations:poisson",
        "//lib/NavierStokesEquations:solver",
        "//gtest:gtest",
    ],
)

cc_test(
    name="diagnosticsTest",
    srcs=[
        "diagnosticsTest.cpp",
    ],
    deps=[
        "//lib/NavierStokesEquations:boundary",
        "//lib/NavierStokesEquations:diagnostics",
        "//lib/NavierStokesEquations:field",
        "//lib/NavierStokesEquations:grid",
        "//lib/NavierStokesEquations:poisson",
        "//lib/NavierStokesEquations:solver",
        "//gtest:gtest",
    ],
)

cc_test(
    name="obstacleTest",
    srcs=[
        "obstacleTest.cpp",
    ],
    deps=[
        "//lib/NavierStokesEquations:field",
        "//lib/NavierStokesEquations:grid",
        "//lib/NavierStokesEquations:obstacle",
        "//gtest:gtest",
    ],
)

cc_test(
    name="ioTest",
    srcs=[
        "ioTest.cpp",
    ],
    deps=[
        "//lib/NavierStokesEquations:diagnostics",
        "//lib/NavierStokesEquations:field",
        "//lib/NavierStokesEquations:grid",
        "//lib/NavierStokesEquations:io",
        "//gtest:gtest",
    ],
)

