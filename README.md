# project-euler

projecteuler.net problems solutions.

We use [Buck2](https://buck2.build/) as a build system.

Freely distributed under MIT licence.

## Building on Ubuntu Linux

The primary supported platform is Ubuntu Linux. To set up a fresh machine:

```sh
./setupUbuntu.sh
```

This installs the APT packages from `ubuntuPackages.txt`, the pip packages from
`ubuntuPipPackages.txt`, and the latest Buck2 release into `~/.local/bin`.

Make sure `~/.local/bin` is on your `PATH`, then build any project with:

```sh
./b.sh Normal           # builds //eulerNormal/... in opt mode
./r.sh Normal           # builds and runs //eulerNormal/...
./b.sh advent/2020/1    # builds //advent/2020/1/...
```

The `cCommon.sh` helper auto-detects `buck2` on PATH. You can override the
binary with `BUCK_BIN=/path/to/buck2`.

## Building on macOS

The same `b.sh` / `r.sh` scripts work on macOS once `buck2` is on PATH (e.g.
`brew install buck2`). The Buck2 toolchain at `toolchains/BUCK` uses the
`system_cxx_toolchain` from the bundled prelude, which picks up `clang` from
PATH on macOS and `g++`/`clang` on Linux.

## Continuous integration

The `.github/workflows/ubuntu-buck2.yml` workflow runs on every push and PR.
It installs the Ubuntu dependency lists, downloads Buck2, and smoke-tests the
build on `ubuntu-latest`.
