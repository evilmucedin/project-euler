#!/usr/bin/env python3

import sys
import os
import subprocess

import argparse

YEAR = 2024

parser = argparse.ArgumentParser(
    description="Prepare directory structure for Advent of Code")
parser.add_argument("--task", type=int,
                    help="task#", required=True)

args = parser.parse_args()
print(f"task = {args.task}")

dr = "%d/%d" % (YEAR, args.task)

if os.path.isdir(dr):
    print("Directory exists")
    sys.exit(1)

os.makedirs(dr)
with open("%s/BUCK" % dr, "w") as fOut:
    print(f"""
cxx_binary(
  name="{args.task}",
  srcs=[
    "{args.task}.cpp",
  ],
  deps=[
    "//lib:header",
    "//lib:init",
    "//lib:string",
    "//gflags:gflags",
    "//advent/lib:aoc",
    "//glog:glog",
  ],
)
""", file=fOut)

with open("%s/%d.cpp" % (dr, args.task), "w") as fOut:
    print("""#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
    """, file=fOut)

with open("%s/TASK" % dr, "w") as fOut:
    print(f"""{args.task}""", file=fOut)

subprocess.check_output(["git", "add", "%s/%d.cpp" % (dr, args.task), "%s/TASK" % dr, "%s/BUCK" % dr])
