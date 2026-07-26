#!/usr/bin/env bash
set -euo pipefail
# Default local coding model (override with CODEX_OLLAMA_MODEL).
DEFAULT_MODEL="${CODEX_OLLAMA_MODEL:-qwen2.5-coder:7b}"
OLLAMA_HOST="${OLLAMA_HOST:-http://localhost:11434}"
usage() {
  cat <<EOF
Usage: $(basename "$0") [codex options...]
Launch Codex CLI against a local Ollama coding model.
Environment:
  CODEX_OLLAMA_MODEL   Ollama model tag (default: ${DEFAULT_MODEL})
  OLLAMA_HOST          Ollama base URL   (default: ${OLLAMA_HOST})
Examples:
  $(basename "$0")
  $(basename "$0") -m qwen2.5-coder:14b
  $(basename "$0") exec "explain this repo"
  CODEX_OLLAMA_MODEL=deepseek-coder-v2 $(basename "$0")
EOF
}
if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi
if ! command -v codex >/dev/null 2>&1; then
  echo "error: codex CLI not found. Install with: npm install -g @openai/codex" >&2
  exit 1
fi
if ! command -v ollama >/dev/null 2>&1; then
  echo "error: ollama not found in PATH" >&2
  exit 1
fi
# Ensure Ollama is reachable.
if ! curl -sf "${OLLAMA_HOST}/api/tags" >/dev/null; then
  echo "Ollama not reachable at ${OLLAMA_HOST}; starting 'ollama serve'..." >&2
  ollama serve >/dev/null 2>&1 &
  for _ in $(seq 1 30); do
    curl -sf "${OLLAMA_HOST}/api/tags" >/dev/null && break
    sleep 1
  done
  curl -sf "${OLLAMA_HOST}/api/tags" >/dev/null || {
    echo "error: Ollama did not become ready at ${OLLAMA_HOST}" >&2
    exit 1
  }
fi
# Pull the model if it is not already present.
if ! ollama list | awk 'NR>1 {print $1}' | grep -qx "${DEFAULT_MODEL}"; then
  echo "Pulling Ollama model: ${DEFAULT_MODEL}" >&2
  ollama pull "${DEFAULT_MODEL}"
fi
# Codex needs a large context window for agent loops (Ollama recommends 64k+).
export OLLAMA_CONTEXT="${OLLAMA_CONTEXT:-65536}"
exec codex --oss --local-provider ollama -m "${DEFAULT_MODEL}" "$@"
