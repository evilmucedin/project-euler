load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)

cmake(
    name = "box2d",
    cache_entries = {
        "BOX2D_BUILD_DOCS": "OFF",
    },
    build_args = ["-j24"],
    lib_source = ":all_srcs",
    out_lib_dir = "lib64",
    out_static_libs = ["libbox2d.a"],
    visibility = ["//visibility:public"],
)