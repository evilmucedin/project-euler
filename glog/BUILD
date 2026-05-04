# load(":bazel/glog.bzl", "glog_library")
# load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

# package(default_visibility = ["//glog"])

# glog_library()

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
    "flags.h",
    "platform.h",
    "types.h",
  ],
  srcs=[
#    "demangle.cc",
    "logging.cc",
    "raw_logging.cc",
    "signalhandler.cc",
    "symbolize.cc",
    "utilities.cpp",
    "vlog_is_on.cc",
    "port.cc",
  ],
  visibility = ["//visibility:public"],
  copts = [
    "-fPIC",
    "-D_GLIBCXX_USE_CXX14_ABI=0",
    "-DMY_FEATURE_ENABLED",

        "-Wno-sign-compare",
        "-Wno-unused-function",
        "-Wno-unused-local-typedefs",
        "-Wno-unused-variable",
        # Allows src/logging.cc to determine the host name.
        "-DHAVE_SYS_UTSNAME_H",
        # For src/utilities.cc.
        "-DHAVE_SYS_TIME_H",
        # NOTE: users could optionally patch -DHAVE_UNWIND off if
        # stacktrace dumping is not needed
        "-DHAVE_UNWIND",
        # Enable dumping stacktrace upon sigaction.
        "-DHAVE_SIGACTION",
        # For logging.cc.
        "-DHAVE_PREAD",
        # -DHAVE_MODE_T prevent repeated typedef mode_t leading
        # to emcc compilation failure
        "-DHAVE_MODE_T",
        "-DHAVE_UNISTD_H",


        "-DGLOG_EXPORT=__attribute__((visibility(\\\"default\\\")))",
        "-DGLOG_NO_EXPORT=__attribute__((visibility(\\\"default\\\")))",
        "-DHAVE_POSIX_FADVISE",
        "-DHAVE_SSIZE_T",
        "-DHAVE_SYS_TYPES_H",
        # For src/utilities.cc.
        "-DHAVE_SYS_SYSCALL_H",
        # For src/logging.cc to create symlinks.
        "-fvisibility-inlines-hidden",
        "-fvisibility=hidden",


        "-DHAVE_EXECINFO_H",
        "-DHAVE_LINK_H",
        "-DHAVE_SYMBOLIZE",  # Supported by <link.h>
],
)
