load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
  name="mathTest",
  srcs=[
    "mathTest.cpp",
  ],
  deps=[
    "//lib:header",
    "//lib:math",
    "//gtest:gtest",
  ],
)

cc_test(
  name="headerTest",
  srcs=[
    "headerTest.cpp",
  ],
  deps=[
    "//lib:header",
    "//gtest:gtest",
  ],
)

cc_test(
  name="trieTest",
  srcs=[
    "trieTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:trie",
  ],
)

cc_test(
  name="datetimeTest",
  srcs=[
    "datetimeTest.cpp",
  ],
  deps=[
    "//lib:datetime",
    "//gtest:gtest",
  ],
)

cc_test(
  name="stringTest",
  srcs=[
    "stringTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:string",
  ],
)

cc_test(
  name="fileTest",
  srcs=[
    "fileTest.cpp",
  ],
  data=[
    "utf8test.txt",
  ],
  deps=[
    "//glog2:glog",
    "//lib:file",
    "//gtest:gtest",
  ],
)

cc_test(
  name="radixSortTest",
  srcs=[
    "radixSortTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:header",
    "//lib:radixSort",
    "//lib:timer",
  ],
)

cc_test(
  name="primesTest",
  srcs=[
    "primesTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:header",
    "//lib:primes",
  ],
)

cc_test(
  name="segmentTreeTest",
  srcs=[
    "segmentTreeTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:header",
    "//lib:segmentTree",
  ],
)

cc_test(
  name="redBlackTreeTest",
  srcs=[
    "redBlackTreeTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:header",
    "//lib:redBlackTree",
  ],
)

cc_test(
  name="integrateTest",
  srcs=[
    "integrateTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:integrate",
  ],
)

cc_test(
  name="fftTest",
  srcs=[
    "fftTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:fft",
    "//lib:random",
  ],
)

cc_test(
  name="progressBarTest",
  srcs=[
    "progressBarTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:progressBar",
  ],
)

cc_test(
  name="exceptionTest",
  srcs=[
    "exceptionTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:exception",
  ],
)

cc_test(
  name="lruTest",
  srcs=[
    "lruTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:lru",
  ],
)

cc_test(
  name="rmqTest",
  srcs=[
    "rmqTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:random",
    "//lib:rmq",
  ],
)

cc_test(
  name="fractionTest",
  srcs=[
    "fractionTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:random",
    "//lib:fraction",
  ],
)

cc_test(
  name="treapTest",
  srcs=[
    "treapTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:random",
    "//lib:treap",
  ],
)

cc_test(
  name="implicitTreapTest",
  srcs=[
    "implicitTreapTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:random",
    "//lib:implicitTreap",
  ],
)

cc_test(
  name="atkinSieveTest",
  srcs=[
    "atkinSieveTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:random",
    "//lib:atkinSieve",
    "//lib:primes",
  ],
)

cc_test(
  name="robinHoodTest",
  srcs=[
    "robinHoodTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:header",
    "//lib:random",
    "//lib:robinHood",
    "//lib:timer",
  ],
)

cc_test(
  name="quickSortTest",
  srcs=[
    "quickSortTest.cpp",
  ],
  deps=[
    "//gtest:gtest",
    "//lib:header",
    "//lib:quickSort",
  ],
)

cc_test(
  name="yfinanceTest",
  srcs=[
    "yfinanceTest.cpp",
  ],
  deps=[
    "//lib:header",
    "//lib:yfinance2",
    "//gtest:gtest",
  ],
  copts = ["-std=c++23"],
  linkopts = ["-lcurl", "-lcpr"],
)
