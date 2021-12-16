#!/usr/bin/env python3

import os
import hashlib
import subprocess

task = open('TASK').readline().strip()
cpp = f"{task}.cpp"

def cs():
    while not os.path.isfile(cpp):
        time.sleep(0.1)
    with open(cpp, "rb") as fIn:
        return hashlib.md5(fIn.read()).hexdigest()

oldCs = 1
while True:
    newCs = cs()
    if newCs != oldCs:
        subprocess.call(f"../../run.py", shell=True, stderr=subprocess.STDOUT)
        oldCs = newCs
