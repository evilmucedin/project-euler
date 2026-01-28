load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
  name="binarySerialization",
  hdrs=[
    "binarySerialization.h",
  ],
  srcs=[
    "binarySerialization.cpp",
  ],
  deps=[
    ":stream",
  ],
  visibility = ["//visibility:public"],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
)

cc_library(
  name="fileIo",
  hdrs=[
    "fileIo.h",
  ],
  srcs=[
    "fileIo.cpp",
  ],
  deps=[
    "//lib:header",
    "//lib:exception",
    "//lib:noncopyable",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0"],
)

# cxx_library(
#   name="fstreamDeprecated",
#   exported_headers=[
#     "fstreamDeprecated.h",
#   ],
#   srcs=[
#      "fstreamDeprecated.cpp",
#   ],
#   deps=[
#     "//lib:header",
#   ],
#   visibility=[
#     "PUBLIC",
#   ],
# )

# cxx_library(
#   name="zstreamDeprecated",
#   exported_headers=[
#     "zstreamDeprecated.h",
#   ],
#   srcs=[
#      "zstreamDeprecated.cpp",
#   ],
#   deps=[
#     "//lib:header",
#     "//zlib2:zlib",
#   ],
#   visibility=[
#     "PUBLIC",
#   ],
# )

cc_library(
  name="zstream",
  hdrs=[
    "zstream.h",
  ],
  srcs=[
     "zstream.cpp",
  ],
  deps=[
    "//lib:header",
    ":stream",
    "//zlib:zlib2",
  ],
  visibility = ["//visibility:public"],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
)

cc_library(
  name="csv",
  hdrs=[
    "csv.h",
  ],
  srcs=[
    "csv.cpp",
  ],
  deps=[
    "//lib:exception",
    "//lib:header",
    ":stream",
    "//lib:string",
  ],
  visibility = ["//visibility:public"],
  features = ["-lto"],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0"],
)

cc_library(
  name="utils",
  hdrs=[
    "utils.h",
  ],
  srcs=[
    "utils.cpp",
  ],
  deps=[
    "//glog:glog2",
    "//lib:header",
    "//lib:string",
    ":stream",
  ],
  visibility = ["//visibility:public"],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
)

cc_library(
  name="varlen",
  hdrs=[
    "varlen.h"
  ],
  srcs=[
    "varlen.cpp",
  ],
  deps=[
    ":fileIo",
  ],
  features = ["-lto"],
  visibility = ["//visibility:public"],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
)

cc_library(
  name="structFile",
  hdrs=[
    "structFile.h",
  ],
  deps=[
    "//lib/io:fileIo",
  ],
  visibility = ["//visibility:public"],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
)

cc_library(
  name="stream",
  hdrs=[
    "stream.h",
  ],
  srcs=[
    "stream.cpp",
  ],
  deps=[
    ":fileIo",
    "//lib:exception",
  ],
  features = ["-lto"],
  visibility = ["//visibility:public"],
  # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
)
