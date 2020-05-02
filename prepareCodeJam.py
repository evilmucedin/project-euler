#!/usr/bin/env python3

import sys
import os

import argparse

parser = argparse.ArgumentParser(
    description="Prepare directory structure for Code Jam")
parser.add_argument("--dir", type=str,
                    help="directory to create", required=True)

args = parser.parse_args()
print(args.dir)

os.makedirs(args.dir)
for problem in ["a", "b", "c", "d"]:
    problemDir = "%s/%s" % (args.dir, problem)
    os.makedirs(problemDir)
    with open("%s/makefile" % problemDir, "w") as fOut:
        print(f"""
{problem}: {problem}.cpp
	g++ -o {problem} -O3 -march=native {problem}.cpp

test: {problem}
	./{problem} <input.txt

submit:
	gedit {problem}.cpp

{problem}.dbg: {problem}.cpp
	g++ -o {problem}.dbg -O0 -g {problem}.cpp
               """, file=fOut)
    with open("%s/%s.cpp" % (problemDir, problem), "w") as fOut:
        print("""#include <cstdio>
#include <cmath>

#include <algorithms>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

using namespace std;

int main() {
    int nT;
    scanf("%d", &nT);

    for (int iTest = 0; iTest < nT; ++iTest) {
        printf("Case #%d: ", iTest + 1);
        printf("\\n");
    }

    return 0;
}
               """, file=fOut)
