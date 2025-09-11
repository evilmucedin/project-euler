load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

# package(default_visibility = ["//glog"])

cc_library(
  name="glog2",
  hdrs=[
    "base/commandlineflags.h",
    "base/googleinit.h",
    "base/mutex.h",
    "config.h",
    "demangle.h",
    "logging.h",
    "log_severity.h",
    "raw_logging.h",
    "stl_logging.h",
    "vlog_is_on.h",
    "mock-log.h",
    "stacktrace_generic-inl.h",
    "stacktrace.h",
    "stacktrace_libunwind-inl.h",
    "stacktrace_powerpc-inl.h",
    "stacktrace_x86_64-inl.h",
    "stacktrace_x86-inl.h",
    "symbolize.h",
    "utilities.h",
  ],
  srcs=[
    "demangle.cc",
    "logging.cc",
    "raw_logging.cc",
    "signalhandler.cc",
    "symbolize.cc",
    "utilities.cpp",
    "vlog_is_on.cc",
    "port.cc",
  ],
  visibility = ["//visibility:public"],
  copts = ["-fPIC"],
)
