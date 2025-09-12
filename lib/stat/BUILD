cc_library(
    name="counters",
    hdrs=[
        "counters.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="stat2",
    hdrs=[
        "stat.h",
    ],
    srcs=[
        "stat.cpp",
    ],
    deps=[
        "//lib:header",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="tdigest",
    hdrs=[
        "tdigest.h",
    ],
    srcs=[
        "tdigest.cpp",
    ],
    deps=[
        "//lib:header",
        "//glog:glog2",
    ],
    visibility = ["//visibility:public"],
)
