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

# Run CatBoost evaluation using local CatBoost repo if available
catboost_script = os.path.join(GDM_ML_DIR, "catboost_run.py")
if os.path.exists(catboost_script):
    print("Running CatBoost evaluator...")
    env_cb = env.copy()
    # Export CATBOOST_REPO and prepend likely python-package path to PYTHONPATH
    env_cb["CATBOOST_REPO"] = CATBOOST_REPO
    cb_python_pkg = os.path.join(CATBOOST_REPO, "python-package")
    env_cb["PYTHONPATH"] = ":".join([cb_python_pkg, CATBOOST_REPO, env_cb.get("PYTHONPATH", "")])
    # Verify we can import catboost with this environment before running the evaluator
    check_cmd = [sys.executable, "-c", "import importlib; importlib.import_module('catboost')"]
    check_res = subprocess.run(check_cmd, cwd=GDM_ML_DIR, env=env_cb, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if check_res.returncode != 0:
        print("CatBoost not importable with current PYTHONPATH; skipping CatBoost step.")
        print("Import error output:\n", check_res.stderr.decode().strip())
        print("If you want CatBoost evaluation, build/install the Python package or set CATBOOST_REPO to a repo with a 'python-package' folder.")
    else:
        # pass the absolute data file path to the script
        data_abs = os.path.join(GDM_ML_DIR, "data.data")
        res_cb = subprocess.run([sys.executable, catboost_script, data_abs], cwd=GDM_ML_DIR, env=env_cb)
        if res_cb.returncode != 0:
            print("WARNING: CatBoost evaluation failed (exit code {} )".format(res_cb.returncode), file=sys.stderr)
else:
    print("CatBoost evaluator not found; skipping CatBoost step.")

print("=== Completed. ML model quality printed above ===")
