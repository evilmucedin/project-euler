#!/usr/bin/env python3

import os
import subprocess
import requests

def main():
    YEAR = 2021

    subprocess.check_output(["buck", "build", "..."])

    task = int(open("TASK").read().strip())
    bn = f"../../../buck-out/gen/advent/{YEAR}/{task}/{task}"
    # print(bn)
    cookies = {"session": "53616c7465645f5f396f77dccd06e249b33e25731f82cc9c72bc1bb12a51e8462e1dcb216cb73e67f06c9b1e3c770b1a"}

    def test(testFilename, testNumber):
        url = f"https://adventofcode.com/{YEAR}/day/{task}/input"
        with open(testFilename, "wb") as fOut:
            fOut.write(requests.get(url, allow_redirects=True, cookies=cookies).content)
        subprocess.check_call(f"{bn} --test={testNumber}", shell=True, stderr=subprocess.STDOUT, stdin=open(testFilename))

    test("first.in", 1)
    test("second.in", 2)

if __name__ == "__main__":
    main()
