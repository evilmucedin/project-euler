exit(0)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Rule repository, note that it's recommended to use a pinned commit to a released version of the rules
http_archive(
   name = "rules_foreign_cc",
   sha256 = "f294fe98f8b9df1264dfb2b6f73225ce68de3246041e86ccf35d19303eed99d6",
   strip_prefix = "rules_foreign_cc-0.4.0",
   url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.4.0.zip",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# This sets up some common toolchains for building targets. For more details, please see
# https://github.com/bazelbuild/rules_foreign_cc/tree/main/docs#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

http_archive(
    name = "box2d",
    build_file = "//deps:box2d.BUILD",
    strip_prefix = "box2d-2.4.1",
    urls = [
        "https://github.com/erincatto/box2d/archive/refs/tags/v2.4.1.tar.gz",
    ],
    sha256 = "d6b4650ff897ee1ead27cf77a5933ea197cbeef6705638dd181adc2e816b23c2",
)

http_archive(
    name = "cgal",
    build_file = "//deps:cgal.BUILD",
    strip_prefix = "CGAL-5.3",
    urls = [
        "https://github.com/CGAL/cgal/releases/download/v5.3/CGAL-5.3.tar.xz",
    ],
    sha256 = "2c242e3f27655bc80b34e2fa5e32187a46003d2d9cd7dbec8fbcbc342cea2fb6",
)

http_archive(
    name = "json",
    build_file = "//deps:json.BUILD",
    urls = [
        "https://github.com/nlohmann/json/releases/download/v3.9.1/include.zip",
    ],
    sha256 = "6bea5877b1541d353bd77bdfbdb2696333ae5ed8f9e8cc22df657192218cad91",
)

http_archive(
    name = "xtl",
    urls = ["https://github.com/xtensor-stack/xtl/archive/e0f00666d90086bb245ae73abb6123d0e2c1b30b.zip"],
    sha256 = "7c517db8a652a965c02a9e37ca3c3a8c7f5ba87868a02fe5a678eb4256b42d0b",
    strip_prefix = "xtl-e0f00666d90086bb245ae73abb6123d0e2c1b30b",
    build_file = "//deps:xtl.BUILD",
)

http_archive(
    name = "xtensor",
    urls = ["https://github.com/xtensor-stack/xtensor/archive/54dbb8223d5a874261e8e988b487054d2e50710e.zip"],
    sha256 = "63ae5657e14f28a2375cc70b6dd0deb10247cb2010055a74fb6de93511735999",
    strip_prefix = "xtensor-54dbb8223d5a874261e8e988b487054d2e50710e",
    build_file = "//deps:xtensor.BUILD",
)

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

http_archive(
    name = "gflags",
    urls = ["https://github.com/gflags/gflags/archive/refs/tags/v2.2.2.zip"],
    sha256 = "19713a36c9f32b33df59d1c79b4958434cb005b5b47dc5400a7a4b078111d9b5",
    strip_prefix = "gflags-2.2.2",
)

http_archive(
    name = "com_github_google_glog",
    sha256 = "21bc744fb7f2fa701ee8db339ded7dce4f975d0d55837a97be7d46e8382dea5a",
    strip_prefix = "glog-0.5.0",
    urls = ["https://github.com/google/glog/archive/v0.5.0.zip"],
)

http_archive(
    name = "imgui",
    urls = ["https://github.com/ocornut/imgui/archive/refs/tags/v1.83.tar.gz"],
    strip_prefix = "imgui-1.83",
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "5a2bde336dc9089025003826ba39af2a27d8b90e",
    remote = "https://github.com/nelhage/rules_boost",
    shallow_since = "1591047380 -0700",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

http_archive(
    name = "pybind11_bazel",
    strip_prefix = "pybind11_bazel-26973c0ff320cb4b39e45bc3e4297b82bc3a6c09",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/26973c0ff320cb4b39e45bc3e4297b82bc3a6c09.zip"],
    sha256 = "a5666d950c3344a8b0d3892a88dc6b55c8e0c78764f9294e806d69213c03f19d",
)
# We still require the pybind library.
http_archive(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    strip_prefix = "pybind11-2.6.2",
    urls = ["https://github.com/pybind/pybind11/archive/refs/tags/v2.6.2.tar.gz"],
    sha256 = "8ff2fff22df038f5cd02cea8af56622bc67f5b64534f1b83b9f133b8366acff2",
)
load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")


register_toolchains("//toolchain:cc_toolchain")
