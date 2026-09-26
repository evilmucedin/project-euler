# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK.

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    hdrs = [
        "base/attributes.h",
        "base/config.h",
        "base/options.h",
        "base/policy_checks.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "functional",
    hdrs = ["functional/function_ref.h"],
    deps = [":base"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "hash",
    hdrs = ["hash/hash.h"],
    deps = [":functional"],
    visibility = ["//visibility:public"],
)
