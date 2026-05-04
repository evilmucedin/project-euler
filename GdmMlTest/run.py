#!/usr/bin/env python3
import os
import subprocess
import sys
import shutil

GDM_ML_DIR = os.path.dirname(os.path.abspath(__file__))
BASE = os.path.abspath(os.path.join(GDM_ML_DIR, ".."))
TARGET = "//GdmMlTest:GdmMlTest"
INJECTED_REPO = "/home/denplusplus/Programming/LightGBM"
DATA_FILE = os.path.join("GdmMlTest", "data.data")
CATBOOST_REPO = "/home/denplusplus/Programming/catboost"

print("=== Bazel GdmMlTest with LightGBM from build_gpp ===")
print(f"Workspace base: {BASE}")

<<<<<<< HEAD
=======
<<<<<<< HEAD
=======
>>>>>>> ee168571e320dce3eafc6655b284fad8ae780efb
bazel_env = os.environ.copy()
# Bazel in this environment is Bazelisk-style and may try to download when using a "latest" style config.
# Pin to a cached version so `run.py` works in a network-restricted sandbox.
bazel_env["USE_BAZEL_VERSION"] = bazel_env.get("USE_BAZEL_VERSION", "9.0.1")
bazel_env["BAZELISK_SKIP_WRAPPER"] = bazel_env.get("BAZELISK_SKIP_WRAPPER", "true")
#
# Bazel needs a writable output base directory for its lock files.
# In this sandbox, the default home/cache may not be writable.
output_base = os.path.join(GDM_ML_DIR, ".bazel_output_base")
os.makedirs(output_base, exist_ok=True)
bazel_env["BAZEL_OUTPUT_BASE"] = output_base

# Also redirect HOME/XDG_CACHE_HOME so Bazel doesn't try to lock/create under ~/.cache.
bazel_home = os.path.join(GDM_ML_DIR, ".bazel_home")
os.makedirs(bazel_home, exist_ok=True)
xdg_cache_home = os.path.join(bazel_home, ".cache")
os.makedirs(xdg_cache_home, exist_ok=True)
bazel_env["HOME"] = bazel_home
bazel_env["XDG_CACHE_HOME"] = xdg_cache_home

# Keep using Bazelisk's existing cached Bazel binaries so we don't hit the network.
bazel_env["BAZELISK_HOME"] = "/home/denplusplus/.cache/bazelisk"

install_base = os.path.join(GDM_ML_DIR, ".bazel_install_base")
os.makedirs(install_base, exist_ok=True)

# Bazel refuses to use repo contents cache locations inside the main workspace tree.
# Use /tmp (writable in this sandbox) to avoid that validation.
repo_contents_cache = os.path.join("/tmp", "gdmmltest_repo_contents_cache")
os.makedirs(repo_contents_cache, exist_ok=True)

# Remove stale Bazel server state (server pid) from previous failed runs.
server_dir = os.path.join(output_base, "server")
shutil.rmtree(server_dir, ignore_errors=True)

<<<<<<< HEAD
=======
>>>>>>> 5dd2c62abcc99357697d23886080f074089f813f
>>>>>>> ee168571e320dce3eafc6655b284fad8ae780efb
# Ensure data.data exists with 100+ features for learning and testing
gen = os.path.join(GDM_ML_DIR, "generate_data.py")
print("Generating/updating data.data...")
subprocess.run([sys.executable, gen], cwd=GDM_ML_DIR, check=True)

<<<<<<< HEAD
=======
<<<<<<< HEAD
cmd_clean = ["bazel", "clean", "--expunge"]
print("Running:", " ".join(cmd_clean))
subprocess.run(cmd_clean, cwd=BASE, check=True)
=======
>>>>>>> ee168571e320dce3eafc6655b284fad8ae780efb
# Bazel's "clean --expunge" tends to touch output-cache lock files that may be unwritable
# in this sandbox. We skip it; the subsequent build/run will compile as needed.

print("Running gdmMlTest via ./run.sh (no Bazel) ...")
res = subprocess.run([os.path.join(GDM_ML_DIR, "run.sh")], cwd=GDM_ML_DIR)
sys.exit(res.returncode)
<<<<<<< HEAD
=======
>>>>>>> 5dd2c62abcc99357697d23886080f074089f813f
>>>>>>> ee168571e320dce3eafc6655b284fad8ae780efb

cmd_run = [
    "bazel",
    f"--output_base={output_base}",
    f"--install_base={install_base}",
    "run",
    "--repo_contents_cache",
    repo_contents_cache,
    "--enable_bzlmod=false",
    "--verbose_failures",
    "--sandbox_debug",
    f"--inject_repository=lightgbm_local={INJECTED_REPO}",
    TARGET,
    "--",
    DATA_FILE,
]
print("Running:", " ".join(cmd_run))

env = bazel_env.copy()
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
