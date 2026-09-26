#!/usr/bin/env python3
"""Generate Bazel BUILD targets from Buck2 BUCK definitions.

Walks the repo like scripts/generate_ninja.py and, for every BUCK file,
converts its cxx_library / cxx_binary / cxx_test targets to rules_cc
cc_library / cc_binary / cc_test with the same names and dependency labels:

  - a directory with a BUCK file but no BUILD file gets a new BUILD file;
  - a directory that already has a (hand-written) BUILD file gets only the
    targets it is missing appended at the end, so existing Bazel-specific
    definitions are never rewritten.

BUILD files that start with the "Generated from BUCK" header are rewritten
from scratch on every run; running the script again is a no-op unless BUCK
files changed.
A BUILD file containing a "# generate_bazel: skip" line is left alone, e.g.
when its targets come from macros the script cannot see.

Attribute mapping:
  srcs, headers                       -> srcs
  exported_headers                    -> hdrs
  deps, exported_deps                 -> deps
  compiler_flags, preprocessor_flags  -> copts
  exported_preprocessor_flags         -> defines (-D) / includes (-I) / copts
  linker_flags, exported_linker_flags -> linkopts
  public_include_directories          -> includes
  visibility = ["PUBLIC"]             -> ["//visibility:public"]

Bazel compiles in a sandbox, so headers that a target includes with a
same-directory relative path ("foo.h") must be declared. Buck2 builds find
them next to the source file anyway; for Bazel such undeclared headers are
added to srcs. An include of another package's header, either relative
("../header.h") or from the repo root ("lib/header.h"), adds a dependency on
the BUCK target exporting that header instead.

Usage:
  python3 scripts/generate_bazel.py           # write BUILD files
  python3 scripts/generate_bazel.py --check   # exit 1 if anything would change
"""

from __future__ import annotations

import ast
import os
import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Set, Union

REPO_ROOT = Path(__file__).resolve().parent.parent
GENERATED_HEADER = "# Generated from BUCK by scripts/generate_bazel.py; keep target names and deps in sync with BUCK."
SKIP_MARKER = "# generate_bazel: skip"
APPENDED_MARKER = "# Targets below were generated from BUCK by scripts/generate_bazel.py."

KINDS = {
    "cxx_library": "cc_library",
    "cxx_binary": "cc_binary",
    "cxx_test": "cc_test",
}

EXCLUDE_DIRS = {
    ".git",
    "buck-out",
    "build-ninja",
    "bazel-bin",
    "bazel-out",
    "bazel-testlogs",
    "bazel-project-euler",
    "toolchains",
    "prelude",
    "node_modules",
    "__pycache__",
}

HEADER_EXTENSIONS = (".h", ".hh", ".hpp", ".hxx", ".inl", ".ipp", ".tcc")
INCLUDE_RE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.MULTILINE)
# <dir/header.h> includes that may name a header of this repo.
ANGLE_INCLUDE_RE = re.compile(r'^\s*#\s*include\s*<([^>]+/[^>]+)>', re.MULTILINE)


class Glob(str):
    """A glob(...) call, copied verbatim: Bazel's glob() has the same syntax."""


# A file list is either literal paths or a glob.
Files = Union[List[str], Glob]


@dataclass
class BuckTarget:
    kind: str
    name: str
    srcs: Files = field(default_factory=list)
    headers: Files = field(default_factory=list)
    exported_headers: Files = field(default_factory=list)
    deps: List[str] = field(default_factory=list)
    copts: List[str] = field(default_factory=list)
    exported_flags: List[str] = field(default_factory=list)
    linkopts: List[str] = field(default_factory=list)
    includes: List[str] = field(default_factory=list)
    public: bool = False


def _strings(node: Optional[ast.AST], constants: Dict[str, object]) -> List[str]:
    """Strings of a list / dict literal (dict values = header paths)."""
    if node is None:
        return []
    if isinstance(node, ast.Name) and node.id in constants:
        node = ast.parse(repr(constants[node.id]), mode="eval").body
    try:
        value = ast.literal_eval(node)
    except ValueError:
        print(f"warn: cannot evaluate {ast.dump(node)[:80]}", file=sys.stderr)
        return []
    if isinstance(value, dict):
        value = list(value.values())
    result = []
    for item in value:
        # srcs entries may be ("file.cpp", ["-flag", ...]) tuples.
        if isinstance(item, (tuple, list)) and item and isinstance(item[0], str):
            item = item[0]
        if isinstance(item, str):
            result.append(item)
    return result


def _norm(path: str) -> str:
    return os.path.normpath(path).replace(os.sep, "/")


def _files(node: Optional[ast.AST], constants: Dict[str, object]) -> Files:
    if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and node.func.id == "glob":
        return Glob(ast.unparse(node).replace("'", '"'))
    return [_norm(s) for s in _strings(node, constants)]


def _flat(files: Files) -> List[str]:
    return [] if isinstance(files, Glob) else list(files)


def loaded_constants(tree: ast.Module) -> Dict[str, object]:
    """Values of load("//pkg:file.bzl", "name") symbols that are plain literals."""
    constants: Dict[str, object] = {}
    for node in tree.body:
        call = node.value if isinstance(node, ast.Expr) else None
        if not (isinstance(call, ast.Call) and isinstance(call.func, ast.Name) and call.func.id == "load"):
            continue
        label = ast.literal_eval(call.args[0])
        if not label.startswith("//"):
            continue
        bzl = REPO_ROOT / label[2:].replace(":", "/")
        wanted = {ast.literal_eval(a) for a in call.args[1:]}
        for stmt in ast.parse(bzl.read_text(encoding="utf-8")).body:
            if isinstance(stmt, ast.Assign) and len(stmt.targets) == 1 and isinstance(stmt.targets[0], ast.Name):
                name = stmt.targets[0].id
                if name in wanted:
                    constants[name] = ast.literal_eval(stmt.value)
    return constants


def parse_buck(path: Path) -> List[BuckTarget]:
    tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
    constants = loaded_constants(tree)
    targets = []
    for node in tree.body:
        if not (isinstance(node, ast.Expr) and isinstance(node.value, ast.Call)):
            continue
        call = node.value
        if not isinstance(call.func, ast.Name) or call.func.id not in KINDS:
            continue
        kw = {k.arg: k.value for k in call.keywords if k.arg}
        name = ast.literal_eval(kw["name"])
        targets.append(BuckTarget(
            kind=call.func.id,
            name=name,
            srcs=_files(kw.get("srcs"), constants),
            headers=_files(kw.get("headers"), constants),
            exported_headers=_files(kw.get("exported_headers"), constants),
            deps=_strings(kw.get("deps"), constants) + _strings(kw.get("exported_deps"), constants),
            copts=_strings(kw.get("compiler_flags"), constants) + _strings(kw.get("preprocessor_flags"), constants),
            exported_flags=_strings(kw.get("exported_preprocessor_flags"), constants),
            linkopts=(_strings(kw.get("linker_flags"), constants)
                      + _strings(kw.get("exported_linker_flags"), constants)),
            includes=_strings(kw.get("public_include_directories"), constants),
            public="PUBLIC" in _strings(kw.get("visibility"), constants),
        ))
    return targets


def existing_names(build: Path) -> Set[str]:
    """Names of all rules (and loaded symbols) already present in a BUILD file."""
    try:
        tree = ast.parse(build.read_text(encoding="utf-8"), filename=str(build))
    except SyntaxError:
        # Not plain Python syntax; fall back to a regex.
        return set(re.findall(r'name\s*=\s*"([^"]+)"', build.read_text(encoding="utf-8")))
    names = set()
    for node in ast.walk(tree):
        if isinstance(node, ast.Call):
            for k in node.keywords:
                if k.arg == "name" and isinstance(k.value, ast.Constant):
                    names.add(k.value.value)
    return names


def loaded_symbols(text: str) -> Set[str]:
    symbols = set()
    for match in re.finditer(r"load\(([^)]*)\)", text):
        symbols.update(re.findall(r'"(cc_\w+)"', match.group(1)))
    return symbols


def package_of(path: Path) -> Path:
    """Directory of the closest BUCK / BUILD file above `path`."""
    directory = path.parent
    while directory != REPO_ROOT and not any((directory / f).exists() for f in ("BUCK", "BUILD", "BUILD.bazel")):
        directory = directory.parent
    return directory


def undeclared_headers(pkg_dir: Path, files: List[str], declared: Set[str],
                       outside: Optional[Set[str]] = None) -> List[str]:
    """Same-package headers reached through #include.

    Headers of other packages are collected into `outside` (repo-relative).
    """
    found: List[str] = []
    todo = list(files)
    seen: Set[str] = set()
    while todo:
        rel = todo.pop()
        if rel in seen:
            continue
        seen.add(rel)
        path = pkg_dir / rel
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        for include in INCLUDE_RE.findall(text) + ANGLE_INCLUDE_RE.findall(text):
            candidate = _norm(os.path.join(os.path.dirname(rel), include))
            if not (pkg_dir / candidate).is_file():
                # Repo-root relative ("lib/header.h").
                if not (REPO_ROOT / include).is_file():
                    continue
                candidate = _norm(os.path.relpath(REPO_ROOT / include, pkg_dir))
            path = (pkg_dir / candidate).resolve()
            if package_of(path) != pkg_dir.resolve():
                if outside is not None:
                    outside.add(_norm(os.path.relpath(path, REPO_ROOT)))
                continue
            if candidate not in declared and candidate not in found:
                found.append(candidate)
            todo.append(candidate)
    return sorted(found)


def _absolute(pkg: str, label: str) -> str:
    if label.startswith(":"):
        return f"//{pkg}{label}"
    if ":" not in label:
        # //foo/bar is short for //foo/bar:bar.
        return f"{label}:{label.rsplit('/', 1)[-1]}"
    return label


def label_list(values: List[str]) -> str:
    return "[" + ", ".join(f'"{v}"' for v in values) + "]"


def string_list(key: str, values: Files, indent: str = "    ", rule_names: Set[str] = frozenset()) -> List[str]:
    if isinstance(values, Glob):
        return [f"{indent}{key} = {values},"]
    # Bazel resolves a label to the rule when a file of the same package has
    # the same name ("armadillo"), so such files cannot be listed at all.
    lines = [f"{indent}# {_quote(v)} omitted: the label names a rule, not the file." for v in values if v in rule_names]
    values = [v for v in values if v not in rule_names]
    if not values:
        return lines
    if len(values) == 1:
        return lines + [f"{indent}{key} = {label_list(values)},"]
    lines.append(f"{indent}{key} = [")
    lines += [f'{indent}    {_quote(v)},' for v in values]
    lines.append(f"{indent}],")
    return lines


def _quote(value: str) -> str:
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def convert(pkg: str, pkg_dir: Path, target: BuckTarget, package_targets: Dict[str, BuckTarget],
            header_owners: Dict[str, str]) -> str:
    rule = KINDS[target.kind]
    if isinstance(target.exported_headers, Glob) and not target.srcs and not target.headers:
        # Header-only library described by a glob, e.g. //eigen.
        return "\n".join([f"{rule}(", f'    name = "{target.name}",']
                         + string_list("hdrs", target.exported_headers)
                         + string_list("deps", target.deps)
                         + (['    visibility = ["//visibility:public"],'] if target.public else [])
                         + [")"])
    srcs = _flat(target.srcs) + _flat(target.headers)
    hdrs = _flat(target.exported_headers)
    if target.kind != "cxx_library":
        # Binaries and tests have no exported headers.
        srcs += hdrs
        hdrs = []

    declared = set(srcs) | set(hdrs)
    todo = [d[1:] for d in target.deps if d.startswith(":")]
    visited: Set[str] = set()
    while todo:
        name = todo.pop()
        if name in visited or name not in package_targets:
            continue
        visited.add(name)
        declared |= set(_flat(package_targets[name].exported_headers))
        todo += [d[1:] for d in package_targets[name].deps if d.startswith(":")]
    outside: Set[str] = set()
    extra = undeclared_headers(pkg_dir, srcs + hdrs, declared, outside)
    deps = list(target.deps)
    known = {_absolute(pkg, d) for d in deps} | {f"//{pkg}:{target.name}"}
    for header in sorted(outside):
        owner = header_owners.get(header)
        if owner and owner not in known:
            known.add(owner)
            deps.append(":" + owner.split(":", 1)[1] if owner.startswith(f"//{pkg}:") else owner)
    if target.kind == "cxx_library" and not target.srcs:
        # Header-only library: its private headers are part of the interface.
        hdrs += extra
    else:
        srcs += extra

    copts = list(target.copts)
    defines: List[str] = []
    includes = list(target.includes)
    for flag in target.exported_flags:
        if flag.startswith("-D"):
            defines.append(flag[2:])
        elif flag.startswith("-I"):
            # Buck -I flags are repo-root relative, Bazel includes are package relative.
            inc = flag[2:]
            includes.append(os.path.relpath(inc, pkg or ".").replace(os.sep, "/") if pkg else inc)
        else:
            copts.append(flag)

    lines = [f"{rule}(", f'    name = "{target.name}",']
    lines += string_list("srcs", srcs, rule_names=set(package_targets))
    lines += string_list("hdrs", hdrs, rule_names=set(package_targets))
    lines += string_list("includes", includes)
    lines += string_list("defines", defines)
    lines += string_list("copts", copts)
    lines += string_list("linkopts", target.linkopts)
    lines += string_list("deps", deps)
    if target.public:
        lines.append('    visibility = ["//visibility:public"],')
    lines.append(")")
    return "\n".join(lines)


def header_owners(bucks: List[Path]) -> Dict[str, str]:
    """Repo-relative exported header path -> label of the BUCK target exporting it."""
    owners: Dict[str, str] = {}
    for buck in bucks:
        pkg = _norm(str(buck.parent.relative_to(REPO_ROOT)))
        pkg = "" if pkg == "." else pkg
        for target in parse_buck(buck):
            if target.kind != "cxx_library":
                continue
            for header in _flat(target.exported_headers):
                owners.setdefault(_norm(os.path.join(pkg, header)), f"//{pkg}:{target.name}")
    return owners


def load_line(symbols: Set[str]) -> str:
    ordered = sorted(symbols)
    return 'load("@rules_cc//cc:defs.bzl", ' + ", ".join(f'"{s}"' for s in ordered) + ")"


def walk_buck_files() -> List[Path]:
    """BUCK files tracked by git (so local experiments are skipped)."""
    try:
        listed = subprocess.run(["git", "ls-files", "-z", "--", "BUCK", "*/BUCK"], cwd=REPO_ROOT,
                                check=True, capture_output=True, text=True).stdout.split("\0")
    except (OSError, subprocess.CalledProcessError):
        listed = []
        for dirpath, dirnames, filenames in os.walk(REPO_ROOT):
            dirnames[:] = [d for d in dirnames if d not in EXCLUDE_DIRS and not d.startswith(".")]
            if "BUCK" in filenames:
                listed.append(os.path.relpath(os.path.join(dirpath, "BUCK"), REPO_ROOT))
    return sorted(REPO_ROOT / f for f in listed if f and Path(f).parts[0] not in EXCLUDE_DIRS)


def process(buck: Path, check: bool, owners: Dict[str, str]) -> Optional[str]:
    pkg_dir = buck.parent
    pkg = _norm(str(pkg_dir.relative_to(REPO_ROOT)))
    pkg = "" if pkg == "." else pkg
    targets = parse_buck(buck)
    if not targets:
        return None

    build = pkg_dir / "BUILD"
    if not build.exists() and (pkg_dir / "BUILD.bazel").exists():
        build = pkg_dir / "BUILD.bazel"
    old_text = build.read_text(encoding="utf-8") if build.exists() else ""
    if SKIP_MARKER in old_text:
        return None
    # Files this script created are regenerated; hand-written ones are appended to.
    text = "" if old_text.startswith(GENERATED_HEADER) else old_text
    have = existing_names(build) if text else set()
    missing = [t for t in targets if t.name not in have]
    if not missing:
        return None

    package_targets = {t.name: t for t in targets}
    body = "\n\n".join(convert(pkg, pkg_dir, t, package_targets, owners) for t in missing)
    needed = {KINDS[t.kind] for t in missing}

    if text:
        new_text = text.rstrip("\n") + "\n"
        absent = needed - loaded_symbols(text)
        if absent:
            new_text = load_line(absent) + "\n" + new_text
        if APPENDED_MARKER not in text:
            new_text += "\n" + APPENDED_MARKER + "\n"
        new_text += "\n" + body + "\n"
        action = f"append {len(missing)} target(s) to"
    else:
        new_text = GENERATED_HEADER + "\n\n" + load_line(needed) + "\n\n" + body + "\n"
        action = "regenerate" if old_text else "create"
    if new_text == old_text:
        return None

    rel = build.relative_to(REPO_ROOT)
    if not check:
        build.write_text(new_text, encoding="utf-8")
    return f"{action} {rel}"


def main(argv: List[str]) -> int:
    check = "--check" in argv[1:]
    bucks = walk_buck_files()
    owners = header_owners(bucks)
    changes = [c for c in (process(b, check, owners) for b in bucks) if c]
    for change in changes:
        print(("would " if check else "") + change)
    print(f"{len(changes)} BUILD file(s) {'out of date' if check else 'updated'}")
    return 1 if check and changes else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
