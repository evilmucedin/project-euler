# GdmMlTest: train LightGBM model on data.data (100+ features), report ML quality.
# Runtime libs: run.py sets LD_LIBRARY_PATH to LightGBM/build_gpp.
load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "GdmMlTest",
    srcs = [
        "gdmMlTest.cpp",
    ],
    deps = [
        "//gflags:gflags2",
        "//glog2:glog",
        "//lib:header",
        "//lib:init",
        "//lib/thread-pool:thread-pool2",
        "//lib:random",
        "//lib:timer",
#        "//lib:stat",
#        "//lib/ml:dataframe",
        "@lightgbm_local//:lightgbm",
    ],
    data = ["data.data"],
    copts = ["-std=c++17"],
    features = ["-lto"],
    # Link against the CatBoost model interface shared library (relative path)
<<<<<<< HEAD
    linkopts = ["-lstdc++", "../catboost/libs/model_interface/libcatboostmodel.so"],
=======
    linkopts = ["-lstdc++", "../catboost/libs/model_interface/libcatboostmodel.so", "-ldl"],
>>>>>>> 5dd2c62abcc99357697d23886080f074089f813f
)
