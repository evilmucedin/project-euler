#!/usr/bin/env python3
import os
import subprocess
import sys

GDM_ML_DIR = os.path.dirname(os.path.abspath(__file__))
BASE = os.path.abspath(os.path.join(GDM_ML_DIR, ".."))
TARGET = "//GdmMlTest:GdmMlTest"
INJECTED_REPO = "/home/denplusplus/Programming/LightGBM"
DATA_FILE = os.path.join("GdmMlTest", "data.data")
CATBOOST_REPO = "/home/denplusplus/Programming/catboost"

import buildBazel

(env, cmd_run, ld_path) = buildBazel.doBazel("build")

buildBazel.doBazel("run")

res = subprocess.run(cmd_run, cwd=BASE, env=env)
if res.returncode != 0:
    print("ERROR: bazel run failed", file=sys.stderr)
    sys.exit(res.returncode)

# Run CatBoost evaluation using local CatBoost repo if available
catboost_script = os.path.join(GDM_ML_DIR, "catboost_run.py")
if os.path.exists(catboost_script):
    print("Running CatBoost evaluator...")
    env_cb = env.copy()
    # Prepend local CatBoost repo to PYTHONPATH so the runner can import it
    env_cb["PYTHONPATH"] = ":".join([CATBOOST_REPO, env_cb.get("PYTHONPATH", "")])
    res_cb = subprocess.run([sys.executable, catboost_script], cwd=GDM_ML_DIR, env=env_cb)
    if res_cb.returncode != 0:
        print("WARNING: CatBoost evaluation failed (exit code {} )".format(res_cb.returncode), file=sys.stderr)
else:
    print("CatBoost evaluator not found; skipping CatBoost step.")

print("=== Completed. ML model quality printed above ===")
