#!/usr/bin/env python3

import platform
import shutil
import subprocess
import time
from pathlib import Path
import os

import psutil


def read_spec(path="test2.txt"):
    p = Path(path)
    if not p.exists():
        return "Specification file not found."
    return p.read_text(encoding="utf-8").strip()


def get_system_info():
    return {
        "os": platform.platform(),
        "python": platform.python_version(),
    }


def get_cpu_info():
    freq = psutil.cpu_freq()
    return {
        "physical_cores": psutil.cpu_count(logical=False) or 0,
        "logical_cores": psutil.cpu_count(logical=True) or 0,
        "freq_mhz": round(freq.current, 1) if freq else 0.0,
        "usage_percent": psutil.cpu_percent(interval=0.2),
    }


def get_memory_info():
    vm = psutil.virtual_memory()
    return {
        "total_gb": round(vm.total / (1024**3), 2),
        "available_gb": round(vm.available / (1024**3), 2),
        "used_percent": vm.percent,
    }


def detect_gpu():
    if not shutil.which("nvidia-smi"):
        return [{"name": "No NVIDIA GPU detected", "memory_gb": 0.0}]
    try:
        output = subprocess.check_output(
            ["nvidia-smi", "--query-gpu=name,memory.total", "--format=csv,noheader,nounits"],
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
    except subprocess.CalledProcessError:
        return [{"name": "No NVIDIA GPU detected", "memory_gb": 0.0}]

    gpus = []
    for line in output.splitlines():
        parts = [x.strip() for x in line.split(",")]
        if len(parts) == 2:
            gpus.append({"name": parts[0], "memory_gb": round(float(parts[1]) / 1024.0, 2)})
    return gpus or [{"name": "No NVIDIA GPU detected", "memory_gb": 0.0}]


def cpu_benchmark(seconds=0.4):
    end = time.perf_counter() + seconds
    x = 1
    loops = 0
    while time.perf_counter() < end:
        x = (x * 1664525 + 1013904223) & 0xFFFFFFFF
        loops += 1
    return loops / seconds


def memory_benchmark(size=6_000_000):
    data = bytearray(b"a" * size)
    start = time.perf_counter()
    copy = data[:]
    elapsed = max(time.perf_counter() - start, 1e-6)
    if copy[0] != data[0]:
        return 0.0
    return (size / (1024 * 1024)) / elapsed


def get_ollama_status():
    ollama_path = shutil.which("ollama")
    if not ollama_path:
        return {
            "installed": False,
            "usable": False,
            "path": None,
            "reason": "ollama is not on PATH",
        }

    try:
        subprocess.check_output(["ollama", "list"], text=True, stderr=subprocess.STDOUT, timeout=15)
        return {
            "installed": True,
            "usable": True,
            "path": ollama_path,
            "reason": "ok",
        }
    except subprocess.CalledProcessError as exc:
        detail = (exc.output or "").strip()
        return {
            "installed": True,
            "usable": False,
            "path": ollama_path,
            "reason": detail or "ollama command failed",
        }
    except subprocess.TimeoutExpired:
        return {
            "installed": True,
            "usable": False,
            "path": ollama_path,
            "reason": "ollama list timed out",
        }


def pick_small_local_model():
    if not shutil.which("ollama"):
        return None
    try:
        output = subprocess.check_output(["ollama", "list"], text=True, stderr=subprocess.DEVNULL).strip()
    except subprocess.CalledProcessError:
        return None

    lines = [line.strip() for line in output.splitlines() if line.strip()]
    if len(lines) <= 1:
        return None

    names = [line.split()[0] for line in lines[1:]]
    preferred_prefixes = ("tinyllama", "qwen2.5-coder:0.5b", "gemma:2b", "phi3:mini")
    for prefix in preferred_prefixes:
        for model_name in names:
            if model_name.lower().startswith(prefix):
                return model_name
    return names[0] if names else None


def run_ollama_microbenchmark(timeout_seconds=5800, prompt=None):
    model = pick_small_local_model()
    if not model:
        return {"ran": False, "model": None, "tokens_per_sec": 0.0, "reason": "No local Ollama model found"}

    if prompt is None:
        # Longer prompt gives a more stable throughput estimate than very short completions.
        prompt = (
            "Write 120-160 words about practical Ubuntu laptop performance testing for "
            "CPU, GPU, RAM, and LLM workloads. Keep it factual."
        )
    start = time.perf_counter()
    try:
        output = subprocess.check_output(
            ["ollama", "run", model, prompt],
            text=True,
            stderr=subprocess.DEVNULL,
            timeout=timeout_seconds,
        ).strip()
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired):
        return {
            "ran": False,
            "model": model,
            "tokens_per_sec": 0.0,
            "reason": f"Model run failed or timed out after {timeout_seconds}s",
        }

    elapsed = max(time.perf_counter() - start, 0.001)
    estimated_tokens = max(int(len(output.split()) * 1.3), 1)
    return {
        "ran": True,
        "model": model,
        "tokens_per_sec": estimated_tokens / elapsed,
        "reason": "ok",
    }


def estimate_tokens_per_year(cpu, memory, gpus, cpu_ops, mem_mb_s, has_ollama, ollama_tps=0.0):
    gpu_mem = sum(g["memory_gb"] for g in gpus)
    base = (
        cpu["logical_cores"] * 60
        + memory["total_gb"] * 35
        + gpu_mem * 160
        + (cpu_ops / 10_000)
        + (mem_mb_s * 2)
    )
    if has_ollama:
        base *= 1.1
    if ollama_tps > 0:
        base += ollama_tps * 6
    return int(max(base, 1) * 35_000)


def print_quick_view(system, cpu, gpus, memory, cpu_ops, mem_bw, ollama_status, ollama_bench, bench_timeout, tokens):
    # Compact labels (max 4 chars) for fast terminal reading.
    print("\nQuick view:")
    print(f"OS  : {system['os']}")
    print(f"PY  : {system['python']}")
    print(f"PCOR: {cpu['physical_cores']}")
    print(f"LCOR: {cpu['logical_cores']}")
    print(f"CFRQ: {cpu['freq_mhz']}")
    print(f"CUSE: {cpu['usage_percent']}")
    print(f"COPS: {int(cpu_ops):,}")
    print(f"GRAM: {sum(g['memory_gb'] for g in gpus):.2f}")
    print(f"MTOT: {memory['total_gb']}")
    print(f"MAVL: {memory['available_gb']}")
    print(f"MUSE: {memory['used_percent']}")
    print(f"MBWS: {mem_bw:.2f}")
    print(f"OINS: {ollama_status['installed']}")
    print(f"OUSE: {ollama_status['usable']}")
    print(f"ORAN: {ollama_bench['ran']}")
    print(f"OTPS: {ollama_bench['tokens_per_sec']:.2f}")
    print(f"OTMO: {bench_timeout}")
    print(f"TYR : {tokens:,}")


def main():
    spec = read_spec("test2.txt")
    system = get_system_info()
    cpu = get_cpu_info()
    memory = get_memory_info()
    gpus = detect_gpu()
    cpu_ops = cpu_benchmark()
    mem_bw = memory_benchmark()
    ollama_status = get_ollama_status()
    has_ollama = ollama_status["installed"]
    bench_timeout = int(os.getenv("OLLAMA_BENCH_TIMEOUT", "5800"))
    fast_timeout = max(1, bench_timeout // 10)
    fast_prompt = (
        "Write a short 1 sentence summary of Ubuntu laptop performance testing for "
        "CPU.. Keep it concise."
    )

    fast_ollama_bench = run_ollama_microbenchmark(timeout_seconds=fast_timeout, prompt=fast_prompt) if ollama_status["usable"] else {
        "ran": False,
        "model": None,
        "tokens_per_sec": 0.0,
        "reason": ollama_status["reason"],
    }
    print("\nOllama fast benchmark result:")
    print(f"  ran: {fast_ollama_bench['ran']}")
    print(f"  model: {fast_ollama_bench['model']}")
    print(f"  tokens_per_sec: {fast_ollama_bench['tokens_per_sec']:.2f}")
    print(f"  timeout_sec: {fast_timeout}")
    print(f"  reason: {fast_ollama_bench['reason']}")

    ollama_bench = run_ollama_microbenchmark(timeout_seconds=bench_timeout) if ollama_status["usable"] else {
        "ran": False,
        "model": None,
        "tokens_per_sec": 0.0,
        "reason": ollama_status["reason"],
    }
    print("\nOllama benchmark result:")
    print(f"  ran: {ollama_bench['ran']}")
    print(f"  model: {ollama_bench['model']}")
    print(f"  tokens_per_sec: {ollama_bench['tokens_per_sec']:.2f}")
    print(f"  timeout_sec: {bench_timeout}")
    print(f"  reason: {ollama_bench['reason']}")

    tokens = estimate_tokens_per_year(
        cpu,
        memory,
        gpus,
        cpu_ops,
        mem_bw,
        has_ollama,
        ollama_bench["tokens_per_sec"],
    )

    print("Specification (test2.txt):")
    print(spec)
    print("\nSystem:")
    for k, v in system.items():
        print(f"{k}: {v}")
    print("\nCPU:")
    for k, v in cpu.items():
        print(f"{k}: {v}")
    print(f"benchmark_ops_per_sec: {int(cpu_ops):,}")
    print("\nGPU:")
    for g in gpus:
        print(f"name: {g['name']}")
        print(f"memory_gb: {g['memory_gb']}")
    print("\nMemory:")
    for k, v in memory.items():
        print(f"{k}: {v}")
    print(f"bandwidth_mb_per_sec: {mem_bw:.2f}")
    print(f"\nOllama installed: {ollama_status['installed']}")
    print(f"Ollama usable in current runtime: {ollama_status['usable']}")
    print(f"Ollama path: {ollama_status['path']}")
    print(f"Ollama benchmark ran: {ollama_bench['ran']}")
    print(f"Ollama model: {ollama_bench['model']}")
    print(f"Ollama benchmark timeout_sec: {bench_timeout}")
    print(f"Ollama tokens_per_sec: {ollama_bench['tokens_per_sec']:.2f}")
    if not ollama_bench["ran"]:
        print(f"Ollama benchmark note: {ollama_bench['reason']}")
    print(f"Estimated LLM tokens per year: {tokens:,}")
    print_quick_view(system, cpu, gpus, memory, cpu_ops, mem_bw, ollama_status, ollama_bench, bench_timeout, tokens)


if __name__ == "__main__":
    main()
