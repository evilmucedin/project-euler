load("//bazel/expanded_template:expanded_template.bzl", "expanded_template")

# ------------------------------------------------------------------------------
# Add native rules to configure source files
def gflags_sources(namespace = ["google", "gflags"]):
    expanded_template(
        name = "gflags_declare_h",
        template = "src/gflags_declare.h.in",
        out = "gflags_declare.h",
        substitutions = {
            "@GFLAGS_NAMESPACE@": namespace[0],
            "@(HAVE_STDINT_H|HAVE_SYS_TYPES_H|HAVE_INTTYPES_H|GFLAGS_INTTYPES_FORMAT_C99)@": "1",
            "@([A-Z0-9_]+)@": "0",
        },
    )
    gflags_ns_h_files = []
    for ns in namespace[1:]:
        gflags_ns_h_file = "gflags_{}.h".format(ns)
        expanded_template(
            name = gflags_ns_h_file.replace(".", "_"),
            template = "gflags2/src/gflags_ns.h.in",
            out = gflags_ns_h_file,
            substitutions = {
                "@ns@": ns,
                "@NS@": ns.upper(),
            },
        )
        gflags_ns_h_files.append(gflags_ns_h_file)
    expanded_template(
        name = "gflags_h",
        template = "gflags2/src/gflags.h.in",
        out = "gflags.h",
        substitutions = {
            "@GFLAGS_ATTRIBUTE_UNUSED@": "",
            "@INCLUDE_GFLAGS_NS_H@": "\n".join(["#include \"gflags/{}\"".format(hdr) for hdr in gflags_ns_h_files]),
        },
    )
    expanded_template(
        name = "gflags_completions_h",
        template = "gflags2/src/gflags_completions.h.in",
        out = "gflags_completions.h",
        substitutions = {
            "@GFLAGS_NAMESPACE@": namespace[0],
        },
    )
    hdrs = [":gflags_h", ":gflags_declare_h", ":gflags_completions_h"]
    hdrs.extend([":" + hdr.replace(".", "_") for hdr in gflags_ns_h_files])
    srcs = [
        "//gflags2/src/config.h",
        "//gflags2/src/gflags.cc",
        "//gflags2/src/gflags_completions.cc",
        "//gflags2/src/gflags_reporting.cc",
        "//gflags2/src/mutex.h",
        "//gflags2/src/util.h",
    ] + select({
        "//:x64_windows": [
            "src/windows_port.cc",
            "src/windows_port.h",
        ],
        "//conditions:default": [],
    })
    return [hdrs, srcs]

# ------------------------------------------------------------------------------
# Add native rule to build gflags library
def gflags_library(hdrs = [], srcs = [], threads = 1):
    name = "gflags2"
    copts = [
        "-DGFLAGS_BAZEL_BUILD",
        "-DGFLAGS_INTTYPES_FORMAT_C99",
        "-DGFLAGS_IS_A_DLL=0",
        # macros otherwise defined by CMake configured defines.h file
        "-DHAVE_STDINT_H",
        "-DHAVE_SYS_TYPES_H",
        "-DHAVE_INTTYPES_H",
        "-DHAVE_SYS_STAT_H",
        "-DHAVE_STRTOLL",
        "-DHAVE_STRTOQ",
        "-DHAVE_RWLOCK",
    ] + select({
        "//:x64_windows": [
            "-DOS_WINDOWS",
        ],
        "//conditions:default": [
            "-DHAVE_UNISTD_H",
            "-DHAVE_FNMATCH_H",
            "-DHAVE_PTHREAD",
        ],
    })
    linkopts = []
    if threads:
        linkopts += select({
            "//:android": [],
            "//:x64_windows": [],
            "//conditions:default": ["-lpthread"],
        })
    else:
        name += "_nothreads"
        copts += ["-DNO_THREADS"]
    native.cc_library(
        name = name,
        hdrs = hdrs,
        srcs = srcs,
        copts = copts,
        linkopts = linkopts,
        visibility = ["//visibility:public"],
        include_prefix = "gflags",
    )
