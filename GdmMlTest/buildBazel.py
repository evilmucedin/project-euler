#!/usr/bin/env python3
import os
import subprocess
import sys

def doBazel(cmd):
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
        "build",
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
    # env["CMD_RUN"] = cmd_run
    # env["LD_LIBRARY_PATH"] = ld_path
    return (env, cmd_run, ld_path)

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

if __name__ == "__main__":
    doBazel("build")
