#!/usr/bin/env python3

import os
import subprocess
import requests
import re

import argparse

def main():
    YEAR = 2022

    parser = argparse.ArgumentParser(
        description="Run Advent of Code solutions")
    parser.add_argument("--task", type=int,
                        help="task#", default=1)
    parser.add_argument("--input", type=str,
                        help="input file", default="")
    parser.add_argument("--time", action="store_true",
                        help="time run")

    args = parser.parse_args()

    subprocess.check_output(["buck", "build", "@mode/opt", "..."])

    task = int(open("TASK").read().strip())
    bn = f"../../../buck-out/gen/advent/{YEAR}/{task}/{task}"
    # print(bn)
    cookies = {"session": "53616c7465645f5f5257a9d018977e24575521d6827fc244fb3671034e48da141f5abb3e6e314d78790d2e3a7555ab715d51b2827a68176adb1d674c63063b07"}

    if not args.input:
        if not os.path.isfile("input.txt"):
            url = f"https://adventofcode.com/{YEAR}/day/{task}"
            content = requests.get(url, allow_redirects=True, cookies=cookies).content.decode('utf-8')
            if content:
                if not os.path.isfile("task.html"):
                    with open("task.html", "w") as fOut:
                        fOut.write(content)
                matches = re.findall(r'<pre><code>(.+?)</code></pre>', content)
                if len(matches) > 0:
                    with open("input.txt", "wb") as fOut:
                        fOut.write(matches[0].encode())

    def test(testFilename, testNumber, download):
        if download:
            url = f"https://adventofcode.com/{YEAR}/day/{task}/input"
            content = requests.get(url, allow_redirects=True, cookies=cookies)
            print("Rewrite %s: %d" % (testFilename, content.status_code))
            if content.status_code == 200:
                with open(testFilename, "wb") as fOut:
                    fOut.write(content.content)
            else:
                print("!!! Error in test fetch")
        prefix = ""
        if args.time:
            prefix = "time "
        subprocess.check_call(f"{prefix}{bn} --test={testNumber}", shell=True, stderr=subprocess.STDOUT, stdin=open(testFilename))

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
