cc_library(
    name="gflags2",

  hdrs=[
    "gflags_declare.h",
    "gflags_gflags.h",
    "gflags.h",
    "gflags_completions.h",
    "config.h",
    "defines.h",
    "util.h",
    "mutex.h",
  ],
  srcs=[
    "gflags.cc",
    "gflags_completions.cc",
    "gflags_reporting.cc",
  ],
  visibility = ["//visibility:public"],
  copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
  features = ["-lto"],
)

#cc_binary(
#    name = "foo",
#    srcs = ["main.cc"],
#    deps = [
#        # "@com_github_gflags_gflags//:gflags", # If using git_repository
#        # Or if using Bzlmod:
#        "@gflags//:gflags2",
#    ],
#)
