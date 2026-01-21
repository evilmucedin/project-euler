# load("@bazel_lib//lib:copy_file.bzl", "copy_file")

# copy_file(
#     name="searchIndexFiles",
#     stc=":1.txt",
#     out="1.txt",
# )

cc_binary(
  name="searchIndex",
  srcs=[
    "searchIndex.cpp",
  ],
  deps=[
#    :searchIndexFiles,
    "//lib:header",
    "//lib:init",
    "//lib:string",
    "//gflags:gflags2",
    "//glog2:glog",
  ],
  data = [
    "1.txt",
  ],
#  linkopts = ["-lgmp"],
)

