# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK.

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "headers",
    hdrs = [
        "jemalloc_defs.h",
        "jemalloc.h",
        "jemalloc_macros.h",
        "jemalloc_mangle.h",
        "jemalloc_mangle_jet.h",
        "jemalloc_protos.h",
        "jemalloc_protos_jet.h",
        "jemalloc_rename.h",
        "jemalloc_typedefs.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jemalloc",
    srcs = [
        "arena.c",
        "atomic.c",
        "base.c",
        "bitmap.c",
        "ckh.c",
        "ctl.c",
        "extent.c",
        "extent_dss.c",
        "extent_mmap.c",
        "hash.c",
        "jemalloc.c",
        "large.c",
        "mb.c",
        "mutex.c",
        "nstime.c",
        "pages.c",
        "prng.c",
        "prof.c",
        "rtree.c",
        "spin.c",
        "stats.c",
        "tcache.c",
        "ticker.c",
        "tsd.c",
        "util.c",
        "witness.c",
    ],
    deps = [
        ":headers",
        "//jemalloc/internal:internal",
    ],
    visibility = ["//visibility:public"],
)
