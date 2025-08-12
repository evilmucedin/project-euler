# load("@bazel_tools//src/conditions:os_conditions.bzl", "OS_CONSTRAINTS")
# load("@bazel_tools//src/conditions:windows.bzl", "windows_condition")

cc_binary(
    name="findStrean",
    srcs=[
        "findStrean.cpp",
    ],
    deps = ["@bazel_tools//tools/cpp/runfiles"],
    data = [
         "input.txt",
         "q.txt",
    ],

#    copts = select({"@bazel_tools//src/conditions:windows": "/std:c++17", ""})),
    copts = ["/std:c++17"],
#     copts = ["-std=c++14"],
#        copts = select({
#            windows_condition: ["/std:c++17"],
#            "//conditions:default": ["-std=c++17"],
#        }),
)

cc_binary(
    name="test",
    srcs=[
        "test.cpp",
    ],
)
