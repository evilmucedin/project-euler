# load("@bazel_tools//src/conditions:os_conditions.bzl", "OS_CONSTRAINTS")

cc_binary(
    name="findStrean",
    srcs=[
        "findStrean.cpp",
    ],
#    deps = [
#    	"//bazel_tools/tools/cpp/runfiles:runfiles",
#    ],
    data = [
         "input.txt",
         "q.txt",
    ],

#    copts = select({"@bazel_tools//src/conditions:windows": "/std:c++17", ""})),
#    copts = ["/std:c++17"],
#     copts = ["-std=c++14"],
)

cc_binary(
    name="test",
    srcs=[
        "test.cpp",
    ],
)
