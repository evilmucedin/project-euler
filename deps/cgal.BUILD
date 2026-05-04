load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

cc_library(
    name = "cgal",
    hdrs = glob(["include/**"]),
    strip_include_prefix = "include/",
    visibility = ["//visibility:public"],
)