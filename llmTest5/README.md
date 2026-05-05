# llmTest5 — LLM Hardware Performance Estimator

This folder contains a set of Python scripts that benchmark a machine's hardware and estimate how many LLM tokens it can generate per year using [Ollama](https://ollama.ai).

---

## What's here

| File | Purpose |
|---|---|
| `test.txt` | Prompt/spec for the first generation (`test1.py`) |
| `test2.txt` | Prompt/spec for the second, more complete generation (`test2.py`) |
| `test.py` | LLM-generated outline — pseudocode-style skeleton, not runnable |
| `test1.py` | First working implementation |
| `test2.py` | Full-featured implementation (the main script) |

---

## What the scripts do

Both `test1.py` and `test2.py` follow the same pipeline:

1. **Collect hardware info** via `psutil` — CPU cores, clock speed, RAM total/available, current usage.
2. **Detect GPU** via `nvidia-smi` — name and VRAM for each NVIDIA GPU (gracefully skips if none found).
3. **Run micro-benchmarks**:
   - *CPU*: tight integer loop for ~0.4 s, reports ops/sec.
   - *Memory*: times a large `bytearray` copy, reports MB/s bandwidth.
4. **Ollama benchmark** (`test2.py` only): finds a small local model (prefers TinyLlama, Qwen, Gemma, Phi), runs it with a fixed prompt, and measures tokens/sec. Runs twice at full length plus 10 short "fast" runs for stability.
5. **Estimate yearly LLM tokens**: a weighted formula combining cores, RAM, GPU VRAM, benchmark scores, and live Ollama throughput.

---

## Running

```bash
# Requires psutil
python3 test2.py
```

Set `OLLAMA_BENCH_TIMEOUT` (seconds) to control how long the Ollama benchmark is allowed to run (default: 5800 s).

---

## Origins

The scripts were generated iteratively by an LLM. `test.txt` and `test2.txt` are the prompts used for each generation round. `test.py` is the raw first-draft outline; `test1.py` and `test2.py` are successively refined, runnable versions.
