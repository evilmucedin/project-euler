load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

# load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
# load("@rules_cc//cc:cc_library.bzl", "cc_library")

# package(default_visibility = ["//lib"])


# load("@pybind11_bazel//:build_defs.bzl", "pybind_library")

cc_library(
    name="lib",
    deps=[
        ":header",
        ":exception",
        ":string",
    ],
    visibility = ["//visibility:public"],
    features = ["-lto"],
)

cc_library(
    name="math",
    srcs=[
    ],
    hdrs=[
        "math.h",
    ],
    visibility = ["//visibility:public"],
    features = ["-lto"],
)

cc_library(
    name="datetime",
    hdrs=[
        "datetime.h",
    ],
    srcs=[
        "datetime.cpp",
    ],
    deps=[
        ":header",
        ":math",
        ":string",
    ],
    visibility = ["//visibility:public"],
    features = ["-lto"],
)

cc_library(
    name="noncopyable",
    srcs=[
        "noncopyable.cpp",
    ],
    hdrs=[
        "noncopyable.h",
    ],
    visibility = ["//visibility:public"],
    features = ["-lto"],
)

cc_library(
    name = "header",
    hdrs = ["header.h"],
    srcs = ["header.cpp"],
    visibility = ["//visibility:public"],
    features = ["-lto"],
)

cc_library(
    name="init",
    hdrs=[
        "init.h",
    ],
    srcs=[
        "init.cpp",
    ],
    deps=[
        ":header",
        "//gflags:gflags2",
        "//glog2:glog",
    ],
    visibility=[
	    "//visibility:public",
    ],
    features = ["-lto"],
)


cc_library(
    name="file",
    hdrs=[
        "file.h",
    ],
    srcs=[
        "file.cpp",
    ],
    deps=[
        ":header",
        ":string",
    ],
    visibility=[
        "//visibility:public",
    ],
    alwayslink=1,
    features = ["-lto"],
    # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
#    alwayslink = 1,
)

cc_library(
    name="string",
    hdrs=[
        "string.h",
    ],
    srcs=[
        "string.cpp",
    ],
    deps=[
        ":header",
        ":exception",
    ],
    visibility=[
	    "//visibility:public",
    ],
    # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
    features = ["-lto"],
#    alwayslink = 1,
)


cc_library(
    name = "matrix",
    hdrs = ["matrix.h"],
    srcs = ["matrix.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
#    alwayslink = 1,
    # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
    features = ["-lto"],
)

cc_library(
    name = "random",
    hdrs = ["random.h"],
    srcs = ["random.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
#    alwayslink = 1,
    # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
    features = ["-lto"],
)

cc_library(
    name = "timer",
    hdrs = ["timer.h"],
    srcs = ["timer.cpp"],
    visibility = ["//visibility:public"],
    deps = [":header"],
    # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
    features = ["-lto"],
#    alwayslink = 1,
)

cc_library(
    name="exception",
    srcs=[
        "exception.cpp",
    ],
    hdrs=[
        "exception.h",
    ],
    deps=[
        ":header",
    ],
    visibility = ["//visibility:public"],
    # copts = ["-D_GLIBCXX_USE_CXX11_ABI=0", "-std=c++17"],
    features = ["-lto"],
#    alwayslink = 1,
)

cc_library(
    name="stat",
    hdrs=[
        "stat.h",
    ],
    deps=[
        ":header",
    ],
    visibility = ["//visibility:public"],
#    alwayslink = 1,
    features = ["-lto"],
)

cc_library(
    name="yfinance2",
    deps=[
        "//lib/yfinance:yfinance2",
    ],
    visibility = ["//visibility:public"],
    features = ["-lto"],
    copts = ["-std=c++23"],
)

# Targets below were generated from BUCK by scripts/generate_bazel.py.

cc_library(
    name = "threads",
    hdrs = ["threads.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "console",
    srcs = ["console.cpp"],
    hdrs = ["console.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "primes",
    srcs = ["primes.cpp"],
    hdrs = ["primes.h"],
    deps = [
        ":header",
        ":math",
        "//glog2:glog",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "radixSort",
    srcs = ["radixSort.cpp"],
    hdrs = ["radixSort.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "segmentTree",
    hdrs = ["segmentTree.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "redBlackTree",
    hdrs = ["redBlackTree.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "integrate",
    srcs = ["integrate.cpp"],
    hdrs = ["integrate.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "fft",
    srcs = ["fft.cpp"],
    hdrs = ["fft.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "progressBar",
    srcs = ["progressBar.cpp"],
    hdrs = ["progressBar.h"],
    deps = [
        ":header",
        "//lib/io:stream",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "geometry",
    srcs = ["geometry.cpp"],
    hdrs = ["geometry.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "convexHull",
    hdrs = ["convexHull.h"],
    deps = [":geometry"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "lru",
    srcs = ["lru.cpp"],
    hdrs = ["lru.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "rmq",
    hdrs = ["rmq.h"],
    deps = [
        ":header",
        "//glog2:glog",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "fraction",
    hdrs = ["fraction.h"],
    deps = [
        ":header",
        "//glog2:glog",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "treap",
    hdrs = ["treap.h"],
    deps = [
        ":header",
        ":random",
        "//glog2:glog",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "implicitTreap",
    hdrs = ["implicitTreap.h"],
    deps = [
        ":header",
        ":random",
        "//glog2:glog",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "benchmark",
    hdrs = ["benchmark.h"],
    deps = [
        ":header",
        ":stat",
        ":timer",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "atkinSieve",
    srcs = ["atkinSieve.cpp"],
    hdrs = ["atkinSieve.h"],
    deps = [
        ":header",
        "//glog2:glog",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "cycles",
    hdrs = ["cycles.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "robinHood",
    hdrs = ["robinHood.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "bigint",
    hdrs = ["bigint.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "unionFind",
    hdrs = ["unionFind.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "unionFindDense",
    hdrs = ["unionFindDense.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "minStack",
    hdrs = ["minStack.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "trie",
    hdrs = ["trie.h"],
    deps = [":exception"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "quickSort",
    hdrs = ["quicksort.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "fenvick",
    srcs = ["fenvick.cpp"],
    hdrs = ["fenvick.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "combinatorics",
    srcs = ["combinatorics.cpp"],
    hdrs = ["combinatorics.h"],
    deps = [":header"],
    visibility = ["//visibility:public"],
)
