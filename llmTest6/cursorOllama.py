#!/usr/bin/env python3
"""
cursorOllama.py — Launch Cursor IDE on Ubuntu configured to use a local Ollama LLM.

Usage:
    python3 cursorOllama.py [--model MODEL] [--ollama-host HOST]
                            [--cursor-path PATH] [--settings PATH]
                            [--no-launch] [workspace]

Environment variables:
    OLLAMA_HOST            Ollama API base URL (default: http://localhost:11434)
    CURSOR_OLLAMA_MODEL    Default model name   (default: qwen2.5-coder:7b)
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
import time
import urllib.request
from pathlib import Path

OLLAMA_HOST = os.getenv("OLLAMA_HOST", "http://localhost:11434").rstrip("/")
DEFAULT_MODEL = os.getenv("CURSOR_OLLAMA_MODEL", "qwen2.5-coder:7b")

# Standard Cursor settings path on Ubuntu (VS Code-style config location).
CURSOR_SETTINGS_DEFAULT = Path.home() / ".config" / "Cursor" / "User" / "settings.json"

# Cursor executables to search for, in priority order.
CURSOR_CANDIDATES = [
    "cursor",
    "/usr/bin/cursor",
    "/usr/local/bin/cursor",
    str(Path.home() / ".local" / "bin" / "cursor"),
    "/opt/cursor/cursor",
    "/snap/bin/cursor",
]


# ---------------------------------------------------------------------------
# Ollama helpers
# ---------------------------------------------------------------------------

def _ollama_get(host: str, path: str, timeout: int = 8) -> dict:
    req = urllib.request.Request(f"{host}{path}")
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return json.loads(resp.read())


def is_ollama_running(host: str) -> bool:
    try:
        _ollama_get(host, "/api/tags")
        return True
    except Exception:
        return False


def start_ollama_service(host: str) -> None:
    if shutil.which("ollama") is None:
        sys.exit("Error: 'ollama' not found in PATH. Install Ollama first:\n"
                 "  curl -fsSL https://ollama.com/install.sh | sh")
    print("Starting Ollama service...")
    subprocess.Popen(
        ["ollama", "serve"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    for _ in range(20):
        time.sleep(1)
        if is_ollama_running(host):
            print("Ollama is running.")
            return
    sys.exit("Error: Ollama did not start within 20 seconds.")


def list_local_models(host: str) -> list[str]:
    try:
        data = _ollama_get(host, "/api/tags")
        return [m["name"] for m in data.get("models", [])]
    except Exception:
        return []


def resolve_model(host: str, requested: str) -> str:
    """Return an exact local model name, pulling from registry if needed."""
    local = list_local_models(host)

    # Exact match first.
    if requested in local:
        return requested

    # Prefix match (e.g. "qwen2.5-coder:7b" matches "qwen2.5-coder:7b-instruct-q4_K_M").
    base = requested.split(":")[0]
    for name in local:
        if name.lower().startswith(base.lower()):
            print(f"Using existing model '{name}' (matched '{requested}').")
            return name

    # Not found locally — pull it.
    print(f"Model '{requested}' not found locally. Pulling from Ollama registry...")
    result = subprocess.run(["ollama", "pull", requested])
    if result.returncode != 0:
        sys.exit(f"Error: failed to pull model '{requested}'.")
    return requested


# ---------------------------------------------------------------------------
# Cursor configuration
# ---------------------------------------------------------------------------

def load_json_safe(path: Path) -> dict:
    if path.exists():
        try:
            return json.loads(path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            pass
    return {}


def configure_cursor_settings(host: str, model: str, settings_path: Path) -> None:
    """
    Merge Ollama OpenAI-compatible endpoint settings into Cursor's settings.json.
    Cursor honours openAI-compatible keys for custom model endpoints.
    """
    settings_path.parent.mkdir(parents=True, exist_ok=True)
    settings = load_json_safe(settings_path)

    v1_base = f"{host}/v1"
    settings["cursor.openAiApiKey"] = "ollama"
    settings["cursor.openAiBaseUrl"] = v1_base
    settings["cursor.defaultModel"] = model
    # Disable telemetry that phones home with prompt data (privacy best-practice).
    settings.setdefault("cursor.telemetry.enabled", False)

    settings_path.write_text(json.dumps(settings, indent=4, ensure_ascii=False), encoding="utf-8")

    print(f"Cursor settings written: {settings_path}")
    print(f"  openAiBaseUrl : {v1_base}")
    print(f"  defaultModel  : {model}")


# ---------------------------------------------------------------------------
# Cursor launcher
# ---------------------------------------------------------------------------

def find_cursor_executable() -> str | None:
    for path in CURSOR_CANDIDATES:
        if shutil.which(path):
            return shutil.which(path)
        if Path(path).is_file() and os.access(path, os.X_OK):
            return path

    # Fall back to AppImage in the home directory.
    for entry in sorted(Path.home().glob("Cursor*.AppImage")):
        if os.access(entry, os.X_OK):
            return str(entry)

    return None


def launch_cursor(executable: str, workspace: str | None) -> None:
    cmd = [executable]
    if workspace:
        cmd.append(workspace)
    print(f"Launching Cursor: {' '.join(cmd)}")
    subprocess.Popen(cmd)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Start Cursor IDE on Ubuntu using a local Ollama LLM.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--model",
        default=DEFAULT_MODEL,
        help="Ollama model name to use.",
    )
    parser.add_argument(
        "--ollama-host",
        default=OLLAMA_HOST,
        metavar="URL",
        help="Ollama API base URL.",
    )
    parser.add_argument(
        "--cursor-path",
        default=None,
        metavar="PATH",
        help="Explicit path to the Cursor executable.",
    )
    parser.add_argument(
        "--settings",
        default=str(CURSOR_SETTINGS_DEFAULT),
        metavar="PATH",
        help="Path to Cursor's settings.json.",
    )
    parser.add_argument(
        "--no-launch",
        action="store_true",
        help="Configure only; do not launch Cursor.",
    )
    parser.add_argument(
        "workspace",
        nargs="?",
        default=None,
        help="Workspace path or file to open in Cursor.",
    )
    args = parser.parse_args()

    # 1. Ensure Ollama is running.
    if is_ollama_running(args.ollama_host):
        print(f"Ollama is already running at {args.ollama_host}.")
    else:
        start_ollama_service(args.ollama_host)

    # 2. Resolve (and optionally pull) the model.
    model = resolve_model(args.ollama_host, args.model)
    print(f"Model ready: {model}")

    # 3. Write Cursor settings.
    configure_cursor_settings(args.ollama_host, model, Path(args.settings))

    if args.no_launch:
        print("Done. Skipping Cursor launch (--no-launch).")
        return

    # 4. Find and launch Cursor.
    cursor_exe = args.cursor_path or find_cursor_executable()
    if not cursor_exe:
        sys.exit(
            "Error: Cursor executable not found.\n"
            "Install Cursor from https://cursor.com or pass --cursor-path."
        )

    launch_cursor(cursor_exe, args.workspace)


if __name__ == "__main__":
    main()
