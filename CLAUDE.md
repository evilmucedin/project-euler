# Claude instructions

Guidance for Claude or other AI coding agents working in this repository.

## Project overview

- This repository contains C++ solutions and experiments for Project Euler, Advent of Code, LeetCode, and related small projects.
- Buck2 is the primary build system. Ninja files are generated from `BUCK` / `BUILD` files by `scripts/generate_ninja.py` and are used for a lightweight CI smoke build. Bazel metadata also exists for targets that have `BUILD` files.
- Many targets depend on optional system libraries such as MKL, OpenGL, CGAL, Boost, or vendored third-party code. Prefer small smoke targets unless the task requires a full target.

## Setup

On Ubuntu, use the repository bootstrap script:

```sh
./setupUbuntu.sh
```

The script installs packages from `ubuntuPackages.txt`, Python packages from `ubuntuPipPackages.txt`, and Buck2 into `~/.local/bin`.

## Common commands

```sh
# Buck2 smoke build used by CI
buck2 build //advent/2020/1/...

# Build a Buck2 project through helper scripts
./b.sh Normal
./b.sh advent/2020/1

# Run a Buck2 project through helper scripts
./r.sh Normal

# Regenerate Ninja files after BUCK/BUILD changes
python3 scripts/generate_ninja.py

# Ninja smoke build used by CI
ninja advent/2020/1/1

# Bazel target build for packages with BUILD files
./bBazel.sh //advent/2024/1:all
```

## Development notes

- Prefer editing `BUCK` / `BUILD` definitions first, then regenerate Ninja files with `python3 scripts/generate_ninja.py` when Ninja build metadata changes. Remember that `BUILD` files may be consumed by Bazel too.
- Do not hand-edit generated `build.ninja` files unless the task is explicitly about generated output; update the generator instead.
- Keep CI smoke builds dependency-light. Expanding CI to full `//...` or full Ninja builds is likely to fail without additional system libraries.
- Existing generated files and third-party directories are large. Keep diffs focused on files relevant to the task.
- Preserve the C++ standard used by the build tooling unless a task specifically requires changing it.

## Before opening a PR

Run the smallest relevant checks. For build-system or setup changes, prefer:

```sh
bash -n setupUbuntu.sh
python3 scripts/generate_ninja.py
ninja advent/2020/1/1
```

If Buck2 is available locally, also run:

```sh
buck2 build //advent/2020/1/...
```
