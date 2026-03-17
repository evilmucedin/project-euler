#!/usr/bin/env python3
import os
import subprocess
import sys

GDM_ML_DIR = os.path.dirname(os.path.abspath(__file__))
BASE = os.path.abspath(os.path.join(GDM_ML_DIR, ".."))
TARGET = "//GdmMlTest:GdmMlTest"
INJECTED_REPO = "/home/denplusplus/Programming/LightGBM"
DATA_FILE = os.path.join("GdmMlTest", "data.data")

print("=== Bazel GdmMlTest with LightGBM from build_gpp ===")
print(f"Workspace base: {BASE}")

# Ensure data.data exists with 100+ features for learning and testing
gen = os.path.join(GDM_ML_DIR, "generate_data.py")
print("Generating/updating data.data...")
subprocess.run([sys.executable, gen], cwd=GDM_ML_DIR, check=True)

cmd_clean = ["bazel", "clean", "--expunge"]
print("Running:", " ".join(cmd_clean))
subprocess.run(cmd_clean, cwd=BASE, check=True)

cmd_run = [
    "bazel",
    "run",
    "--verbose_failures",
    f"--inject_repository=lightgbm_local={INJECTED_REPO}",
    TARGET,
    "--",
    DATA_FILE,
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

print("=== Completed. ML model quality printed above ===")
