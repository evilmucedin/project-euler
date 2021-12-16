#!/usr/bin/env python3

import hashlib
import subprocess

task = open('TASK').readline().strip()
cpp = f"{task}.cpp"

def cs():
    fIn = open(cpp, "rb")
    while not fIn:
        fIn = open(cpp, "rb")
    res = hashlib.md5(fIn.read()).hexdigest()
    fIn.close()
    return res

oldCs = 1
while True:
    newCs = cs()
    if newCs != oldCs:
        subprocess.call(f"../../run.py", shell=True, stderr=subprocess.STDOUT)
        oldCs = newCs
