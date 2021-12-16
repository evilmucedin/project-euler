#!/usr/bin/env python3

import hashlib
import subprocess

task = open('TASK').readline().strip()
cpp = f"{task}.cpp"

def cs():
    return hashlib.md5(open(cpp, "rb").read()).hexdigest()

oldCs = 1
while True:
    newCs = cs()
    if newCs != oldCs:
        subprocess.check_call(f"../../run.py", shell=True, stderr=subprocess.STDOUT)
        oldCs = newCs
