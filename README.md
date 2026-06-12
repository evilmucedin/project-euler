# project-euler

projecteuler.net problems solutions.

We use [Buck2](https://buck2.build/) as the primary build system. The repo also contains generated Ninja files for lightweight smoke builds and Bazel metadata for targets that have `BUILD` files.

Freely distributed under MIT licence.

## Recommendation for apps and libraries

New apps (binaries) and libraries should be buildable and runnable with **all
three** supported build systems — **Bazel**, **Buck2**, and **Ninja** — so they
work in every workflow (local dev, CI smoke builds, and Bazel-based tooling).
Concretely, for each new directory:

1. **Add a `BUCK` file** describing the targets with `cxx_library`,
   `cxx_binary`, or `cxx_test`. Buck2 is the primary build system.
2. **Add a `BUILD` file** with the matching Bazel rules — `cc_library`,
   `cc_binary`, or `cc_test` (load them from `@rules_cc//cc:defs.bzl`). Keep the
   target names and dependency list in sync with the `BUCK` file.
3. **Regenerate the Ninja metadata** with `python3 scripts/generate_ninja.py`,
   which reads the `BUCK` (or `BUILD`) files and emits the per-directory
   `build.ninja` plus the root `build.ninja`. Commit the regenerated files.

Use workspace-root-relative includes (e.g. `#include "bignum/bigint.h"`) so the
same sources compile under every build system, and keep new CI smoke targets
dependency-light. A minimal library + binary then builds and runs like this:

```sh
# Buck2 (primary)
buck2 build //path/to:target
buck2 run   //path/to:binary

# Bazel
bazel build //path/to:target
bazel run   //path/to:binary
# or: ./bBazel.sh //path/to:target

# Ninja (after regenerating with scripts/generate_ninja.py)
ninja path/to/binary
./build-ninja/bin/path/to/binary
```

Tests should be runnable as `cc_test` under Bazel (`bazel test //path/to:...`),
as `cxx_test` / `cxx_binary` under Buck2, and as a Ninja binary target.

## Building on Ubuntu Linux

The primary supported platform is Ubuntu Linux. To set up a fresh machine:

```sh
./setupUbuntu.sh
```

This installs the APT packages from `ubuntuPackages.txt` plus core build tools
(including Ninja), the pip packages from `ubuntuPipPackages.txt` and
`pipPackages.txt`, and the latest Buck2 release into `~/.local/bin`.

Make sure `~/.local/bin` is on your `PATH`, then build any project with:

```sh
./b.sh Normal           # builds //eulerNormal/... in opt mode
./r.sh Normal           # builds and runs //eulerNormal/...
./b.sh advent/2020/1    # builds //advent/2020/1/...
./bETFModelingBuck2.sh   # builds //ETFModeling:ETFModeling with Buck2
./bBazel.sh //advent/2024/1:all  # builds an existing Bazel target
```

The `cCommon.sh` helper uses `buck2` from PATH, `BUCK_BIN`, or the standard
`~/.local/bin/buck2` location used by `setupUbuntu.sh`. You can install only
Buck2 with one of the platform installers:

```sh
./scripts/installBuck2Ubuntu.sh      # Ubuntu Linux
./scripts/installBuck2MacOS.sh       # macOS
powershell -ExecutionPolicy Bypass -File .\\scripts\\installBuck2Windows.ps1
```

You can also override the binary with `BUCK_BIN=/path/to/buck2`. Legacy Buck is
retired for this repository.

Bazel is also available for the subset of the repository that has `BUILD` files.
Use `./bBazel.sh <target>` or call `bazel build <target>` directly. You can set
`BAZEL_BIN=/path/to/bazel` to choose a specific Bazel/Bazelisk binary.

## Building on macOS

The same `b.sh` / `r.sh` scripts work on macOS once `buck2` is on PATH (e.g.
`brew install buck2`). The Buck2 toolchain at `toolchains/BUCK` uses the
`system_cxx_toolchain` from the bundled prelude, which picks up `clang` from
PATH on macOS and `g++`/`clang` on Linux.

## Continuous integration

The `.github/workflows/ubuntu-buck2.yml` and `.github/workflows/ubuntu-ninja.yml`
workflows run on every push and PR. They install the Ubuntu dependency lists,
download Buck2 where needed, and smoke-test the build on `ubuntu-latest`.
