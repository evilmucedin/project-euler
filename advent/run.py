#!/usr/bin/env python3

import os
import subprocess
import requests
import re

import argparse

def main():
    YEAR = 2021

    parser = argparse.ArgumentParser(
        description="Run Advent of Code solutions")
    parser.add_argument("--task", type=int,
                        help="task#", default=1)
    parser.add_argument("--input", type=str,
                        help="input file", default="")

    args = parser.parse_args()

    subprocess.check_output(["buck", "build", "@mode/opt", "..."])

    task = int(open("TASK").read().strip())
    bn = f"../../../buck-out/gen/advent/{YEAR}/{task}/{task}"
    # print(bn)
    cookies = {"session": "53616c7465645f5f396f77dccd06e249b33e25731f82cc9c72bc1bb12a51e8462e1dcb216cb73e67f06c9b1e3c770b1a"}

    if not args.input:
        url = f"https://adventofcode.com/{YEAR}/day/{task}"
        content = requests.get(url, allow_redirects=True, cookies=cookies).content.decode('utf-8')
        matches = re.findall(r'<pre><code>(.+?)</code></pre>', content)
        with open("input.txt", "wb") as fOut:
            fOut.write(matches[0].encode())

    def test(testFilename, testNumber, download):
        if download:
            url = f"https://adventofcode.com/{YEAR}/day/{task}/input"
            with open(testFilename, "wb") as fOut:
                fOut.write(requests.get(url, allow_redirects=True, cookies=cookies).content)
        subprocess.check_call(f"{bn} --test={testNumber}", shell=True, stderr=subprocess.STDOUT, stdin=open(testFilename))

    if args.input:
        test(args.input, args.task, False)
    else:
        test("input.txt", 1, False)
        test("input.txt", 2, False)
        test("first.in", 1, True)
        test("second.in", 2, True)

if __name__ == "__main__":
    main()
