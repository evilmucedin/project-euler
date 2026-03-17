#!/usr/bin/env python3
import os
import subprocess
import sys

BASE = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
TARGET = "//GdmMlTest:GdmMlTest"
INJECTED_REPO = "/home/denplusplus/Programming/LightGBM"

print("=== Bazel GdmMlTest with LightGBM from build_gpp ===")
print(f"Workspace base: {BASE}")

cmd_clean = ["bazel", "clean", "--expunge"]
print("Running:", " ".join(cmd_clean))
subprocess.run(cmd_clean, cwd=BASE, check=True)

cmd_run = [
    "bazel",
    "run",
    "--verbose_failures",
    f"--inject_repository=lightgbm_local={INJECTED_REPO}",
    TARGET,
]
print("Running:", " ".join(cmd_run))

env = os.environ.copy()
ld_path = ":".join([
    os.path.join(INJECTED_REPO, "build_gpp"),
    INJECTED_REPO,
    env.get("LD_LIBRARY_PATH", ""),
])
env["LD_LIBRARY_PATH"] = ld_path

res = subprocess.run(cmd_run, cwd=BASE, env=env)
if res.returncode != 0:
    print("ERROR: bazel run failed", file=sys.stderr)
    sys.exit(res.returncode)

print("=== Completed. Model quality printed above ===")
