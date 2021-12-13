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
        if not os.path.isfile("input.txt"):
            url = f"https://adventofcode.com/{YEAR}/day/{task}"
            content = requests.get(url, allow_redirects=True, cookies=cookies).content.decode('utf-8')
            if content:
                if not os.path.isfile("task.html"):
                    with open("task.html", "w") as fOut:
                        fOut.write(content.encode())
                matches = re.findall(r'<pre><code>(.+?)</code></pre>', content)
                if len(matches) > 0:
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
        if os.path.isfile("input.txt"):
            test("input.txt", 1, False)
            test("input.txt", 2, False)
        test("first.in", 1, True)
        test("second.in", 2, True)

if __name__ == "__main__":
    main()
