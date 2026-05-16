#!/usr/bin/env python3
"""
cursorOllama.py — Launch Cursor IDE on Ubuntu configured to use Ollama via the ngrok
tunnel configured in ~/.bashrc_local (static --url forwarded to localhost:11434).

Usage:
<<<<<<< HEAD
    python3 cursorOllama.py [--model MODEL] [--ollama-host HOST]
                            [--cursor-path PATH] [--settings PATH] [--state-db PATH]
                            [--no-state-patch] [--no-launch] [workspace]

Quit Cursor before running so state.vscdb is not SQLite-locked.
Ollama is started or verified before any Cursor writes; Cursor launches only
after the OpenAI-compatible /v1 endpoint lists the resolved model.
=======
    python3 cursorOllama.py [--model MODEL] [--ollama-host URL]
                            [--cursor-path PATH] [--settings PATH]
                            [--no-launch] [workspace]
>>>>>>> e256e6cbde0541a13aaae58e6ac42471ce238d86

Environment variables:
    OLLAMA_HOST            Public Ollama base URL Cursor uses (ngrok HTTPS URL)
                           (default matches ~/.bashrc_local ngrok --url …)
    CURSOR_OLLAMA_MODEL    Default model name   (default: qwen2.5-coder:7b)

Local port 11434 is used to start/serve Ollama and for model pulls; Cursor’s
API base URL is set to OLLAMA_HOST (the ngrok tunnel) unless overridden.
"""

import argparse
import json
import os
import shutil
import sqlite3
import subprocess
import sys
import time
import urllib.request
from pathlib import Path
from urllib.parse import urlparse

# Matches ~/.bashrc_local: ngrok http 11434 --url https://….
OLLAMA_TUNNEL_HOST = os.getenv(
    "OLLAMA_HOST",
    "https://visor-ion-sizably.ngrok-free.dev",
).rstrip("/")
LOCAL_OLLAMA_HOST = (
    os.getenv("OLLAMA_LOCAL_HOST", "http://localhost:11434").rstrip("/")
)
DEFAULT_MODEL = os.getenv("CURSOR_OLLAMA_MODEL", "qwen2.5-coder:7b")

# Standard Cursor settings path on Ubuntu (VS Code-style config location).
CURSOR_SETTINGS_DEFAULT = Path.home() / ".config" / "Cursor" / "User" / "settings.json"

# Cursor stores AI override fields in SQLite reactive storage — settings.json alone is not enough.
CURSOR_GLOBAL_STORAGE_DEFAULT = Path.home() / ".config" / "Cursor" / "User" / "globalStorage"
CURSOR_STATE_VSCDB_DEFAULT = CURSOR_GLOBAL_STORAGE_DEFAULT / "state.vscdb"
REACTIVE_USER_STORAGE_KEY = (
    "src.vs.platform.reactivestorage.browser.reactiveStorageServiceImpl."
    "persistentStorage.applicationUser"
)

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
    # ngrok free tier returns an HTML interstitial unless this header is set.
    if "ngrok" in host.lower():
        req.add_header("ngrok-skip-browser-warning", "1")
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return json.loads(resp.read())


def is_ollama_running(host: str) -> bool:
    try:
        _ollama_get(host, "/api/tags")
        return True
    except Exception:
        return False


def _ollama_serve_env(api_base_url: str) -> dict[str, str]:
    """
    Maps the HTTP API URL to OLLAMA_HOST=host:port so `ollama serve` listens
    where the script checks (e.g. http://127.0.0.1:11434 → OLLAMA_HOST=127.0.0.1:11434).
    """
    u = urlparse(api_base_url)
    if not u.hostname:
        return {}
    port = u.port or 11434
    merged = dict(os.environ)
    merged["OLLAMA_HOST"] = f"{u.hostname}:{port}"
    return merged


def start_ollama_service(host: str) -> None:
    if shutil.which("ollama") is None:
        sys.exit("Error: 'ollama' not found in PATH. Install Ollama first:\n"
                 "  curl -fsSL https://ollama.com/install.sh | sh")
    print("Starting Ollama service...")
    subprocess.Popen(
        ["ollama", "serve"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        env=_ollama_serve_env(host),
    )
    for _ in range(35):
        time.sleep(1)
        if is_ollama_running(host):
            print("Ollama is running.")
            return
    sys.exit("Error: Ollama did not start within 35 seconds.")


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


def _ollama_openai_models_ids(host: str) -> list[str]:
    req = urllib.request.Request(f"{host}/v1/models")
    with urllib.request.urlopen(req, timeout=8) as resp:
        data = json.loads(resp.read())
    return [m.get("id", "") for m in data.get("data", []) if m.get("id")]


def verify_ollama_before_cursor_launch(host: str, model: str, wait_sec: float = 25.0) -> None:
    """
    Block until Cursor's OpenAI-compat path (/v1) is reachable and lists the model.
    Cursor talks to http://HOST/v1/chat/completions, not only /api/*.
    """
    deadline = time.time() + wait_sec
    last_err: Exception | None = None
    while time.time() < deadline:
        try:
            ids = _ollama_openai_models_ids(host)
            if model in ids:
                print(f"Ollama /v1 API is ready — model '{model}' available for Cursor.")
                return
            # Some installs lag briefly after pull; tolerate empty list then retry.
            if ids and model not in ids:
                local = list_local_models(host)
                if model in local:
                    last_err = RuntimeError(f"/v1/models missing '{model}', have: {ids!r}")
        except Exception as exc:
            last_err = exc
        time.sleep(0.4)
    hint = f" ({last_err})" if last_err else ""
    sys.exit(
        f"Error: Ollama OpenAI-compatible API at {host}/v1 is not ready "
        f"or does not list model '{model}' within {wait_sec}s.{hint}"
    )


def prepare_ollama(host: str, requested_model: str) -> str:
    """
    Start Ollama if needed, ensure the coding model exists, return resolved name.
    Always run this before altering Cursor configuration or launching Cursor.
    """
    if is_ollama_running(host):
        print(f"Ollama is already running at {host}.")
    else:
        start_ollama_service(host)
    model = resolve_model(host, requested_model)
    print(f"Model ready: {model}")
    return model


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
    Cursor also persists the effective override in SQLite (see patch_state_vscdb_openai_base).
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


def patch_state_vscdb_openai_base(v1_base: str, state_db: Path) -> None:
    """
    Writes openAIBaseUrl into Cursor's reactive user blob in state.vscdb.

    Newer Cursor builds keep 'Override OpenAI base URL' in this SQLite store.
    Without it, entries in settings.json are ignored and traffic never reaches Ollama.
    Close Cursor before running if you hit 'database is locked'.
    """
    if not state_db.is_file():
        print(f"Note: Cursor state DB not found ({state_db}); skipping SQLite patch.")
        return

    try:
        con = sqlite3.connect(f"file:{state_db}?mode=rw", uri=True, timeout=5.0)
    except sqlite3.Error as exc:
        print(f"Warning: could not open Cursor state DB for writing: {exc}")
        return

    try:
        cur = con.cursor()
        cur.execute(
            "SELECT value FROM ItemTable WHERE key = ?",
            (REACTIVE_USER_STORAGE_KEY,),
        )
        row = cur.fetchone()
        if not row:
            print(
                "Note: reactive user storage row missing in state.vscdb; "
                "open Cursor once, quit, then re-run this script."
            )
            return

        data = json.loads(row[0])
        if data.get("openAIBaseUrl") == v1_base:
            print(f"Cursor state DB already has openAIBaseUrl = {v1_base}")
            return

        bak = state_db.with_suffix(state_db.suffix + ".bak_cursor_ollama")
        shutil.copy2(state_db, bak)
        print(f"Backed up Cursor state DB to {bak}")

        data["openAIBaseUrl"] = v1_base
        serialized = json.dumps(data, separators=(",", ":"), ensure_ascii=False)
        cur.execute(
            "UPDATE ItemTable SET value = ? WHERE key = ?",
            (serialized, REACTIVE_USER_STORAGE_KEY),
        )
        con.commit()
        print(f"Updated Cursor state DB: openAIBaseUrl -> {v1_base}")
    except sqlite3.OperationalError as exc:
        if "locked" in str(exc).lower():
            print(
                "Warning: Cursor state DB is locked. Quit Cursor completely, "
                "then run:\n"
                f"  python3 cursorOllama.py --no-launch [--settings ... --state-db ...]\n"
                f"Original error: {exc}"
            )
        else:
            print(f"Warning: SQLite update failed: {exc}")
    except json.JSONDecodeError as exc:
        print(f"Warning: could not parse reactive storage JSON in state.vscdb: {exc}")
    finally:
        con.close()


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
        description=(
            "Start Cursor IDE using Ollama through the ngrok URL from ~/.bashrc_local "
            "(OLLAMA_HOST), with localhost for serve/pull."
        ),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--model",
        default=DEFAULT_MODEL,
        help="Ollama model name to use.",
    )
    parser.add_argument(
        "--ollama-host",
        default=OLLAMA_TUNNEL_HOST,
        metavar="URL",
        help=(
            "Public Ollama API base URL for Cursor settings (typically your ngrok URL). "
            "Override with OLLAMA_HOST."
        ),
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
        "--state-db",
        default=str(CURSOR_STATE_VSCDB_DEFAULT),
        metavar="PATH",
        help="Path to Cursor globalStorage/state.vscdb for openAIBaseUrl patch.",
    )
    parser.add_argument(
        "--no-state-patch",
        action="store_true",
        help="Do not patch state.vscdb (only settings.json).",
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
<<<<<<< HEAD
    args.ollama_host = args.ollama_host.rstrip("/")

    # 1–2 Ollama first: daemon + model — Cursor must not start against a broken host.
    model = prepare_ollama(args.ollama_host, args.model)

    # 3 Cursor configuration (written only after Ollama is usable at /api).
    v1_base = f"{args.ollama_host}/v1"
    configure_cursor_settings(args.ollama_host, model, Path(args.settings))
    if not args.no_state_patch:
        patch_state_vscdb_openai_base(v1_base, Path(args.state_db))
=======
    cursor_endpoint = args.ollama_host.rstrip("/")

    # 1. Ensure the local Ollama daemon is running (what ngrok forwards to).
    if is_ollama_running(LOCAL_OLLAMA_HOST):
        print(f"Ollama is already running locally at {LOCAL_OLLAMA_HOST}.")
    else:
        start_ollama_service(LOCAL_OLLAMA_HOST)

    if (
        cursor_endpoint != LOCAL_OLLAMA_HOST
        and not is_ollama_running(cursor_endpoint)
    ):
        print(
            f"\nWarning: Cannot reach Cursor’s OLLAMA_HOST at {cursor_endpoint}.\n"
            "Ensure ngrok is running (same as in ~/.bashrc_local), tunneling "
            f"localhost:11434 to this URL. Cursor settings will still use it.\n"
        )

    # 2. Resolve (and optionally pull) against local daemon (CLI + API).
    model = resolve_model(LOCAL_OLLAMA_HOST, args.model)
    print(f"Model ready: {model}")

    # 3. Write Cursor settings to use the tunnel URL (unless user passed localhost).
    configure_cursor_settings(cursor_endpoint, model, Path(args.settings))
>>>>>>> e256e6cbde0541a13aaae58e6ac42471ce238d86

    if args.no_launch:
        print("Done. Skipping Cursor launch (--no-launch).")
        return

    # 4 Right before Cursor: confirm OpenAI-compat /v1 (what the editor actually calls).
    verify_ollama_before_cursor_launch(args.ollama_host, model)

    # 5 Launch Cursor only after Ollama is verified for that stack.
    cursor_exe = args.cursor_path or find_cursor_executable()
    if not cursor_exe:
        sys.exit(
            "Error: Cursor executable not found.\n"
            "Install Cursor from https://cursor.com or pass --cursor-path."
        )

    launch_cursor(cursor_exe, args.workspace)


if __name__ == "__main__":
    main()
