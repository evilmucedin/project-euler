# BUILD file in your project's root or a subdirectory

load("@bazel_skylib//lib:selects.bzl", "selects")

cc_library(
    name = "glog_lib",
    srcs = glob([
        "src/**/*.cc",
    ]),
    hdrs = glob([
        "src/**/*.h",
    ]),
    copts = selects.with_or({
        "@bazel_tools//src/conditions:windows": [
            "-D_NO_CRT_STDIO_INLINE", # snprintf conflict workaround
            "-I%s/src/windows" % __workspace_dir__,
        ],
        "//conditions:default": [
            "-DHAVE_PTHREAD",
            "-DHAVE_SYS_UTSNAME_H",
            "-DHAVE_SYS_SYSCALL_H",
            "-DHAVE_SYS_TIME_H",
            "-DHAVE_SIGACTION",
            # Add other necessary copts for non-Windows platforms
        ],
    }),
    deps = [
        "@com_github_gflags_gflags//:gflags", # If you are also using gflags
    ],
    visibility = ["//visibility:public"],
)


cc_library(
  name="glog",
  exported_headers=[
    "./base/commandlineflags.h",
    "./base/googleinit.h",
    "./base/mutex.h",
    "./config.h",
    "./demangle.h",
    "./logging.h",
    "./log_severity.h",
    "./raw_logging.h",
    "./stl_logging.h",
    "./vlog_is_on.h",
    "./mock-log.h",
    "./stacktrace_generic-inl.h",
    "./stacktrace.h",
    "./stacktrace_libunwind-inl.h",
    "./stacktrace_powerpc-inl.h",
    "./stacktrace_x86_64-inl.h",
    "./stacktrace_x86-inl.h",
    "./symbolize.h",
    "./utilities.h",
  ],
  srcs=[
    "./demangle.cc",
    "./logging.cc",
    "./raw_logging.cc",
    "./signalhandler.cc",
    "./symbolize.cc",
    "./utilities.cpp",
    "./vlog_is_on.cc",
    "./port.cc",
  ],
  visibility = ["//visibility:public"],
)
