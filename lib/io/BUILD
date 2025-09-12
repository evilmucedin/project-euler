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
    "//zlib2:zlib",
  ],
    visibility = ["//visibility:public"],
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
    visibility = ["//visibility:public"],
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
    visibility = ["//visibility:public"],
)
