# project-euler

projecteuler.net problems solutions.

We use [Buck2](https://buck2.build/) as the primary build system. The repo also contains generated Ninja files for lightweight smoke builds and Bazel metadata for targets that have `BUILD` files.

Freely distributed under MIT licence.

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

The `cCommon.sh` helper requires `buck2` on PATH. You can install only Buck2
with one of the platform installers:

```sh
./installBuck2Ubuntu.sh      # Ubuntu Linux
./installBuck2MacOS.sh       # macOS
powershell -ExecutionPolicy Bypass -File .\\installBuck2Windows.ps1
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
