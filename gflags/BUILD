cc_library(
    name="gflags",

  exported_headers=[
    "gflags_declare.h",
    "gflags_gflags.h",
    "gflags.h",
  ],
  headers=[
    "gflags_completions.h",
  ],
  srcs=[
    "gflags.cc",
    "gflags_completions.cc",
    "gflags_reporting.cc",
  ],
  visibility = ["//visibility:public"],
)

cc_binary(
    name = "foo",
    srcs = ["main.cc"],
    deps = [
        "@com_github_gflags_gflags//:gflags", # If using git_repository
        # Or if using Bzlmod:
        # "@gflags//:gflags",
    ],
)
