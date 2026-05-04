#!/usr/bin/env python3

import subprocess
import time
from pathlib import Path

import psutil


def read_scenario(path="test.txt"):
    file_path = Path(path)
    if not file_path.exists():
        return "No scenario file found."
    return file_path.read_text(encoding="utf-8").strip()


def _read_cpu_model():
    cpuinfo = Path("/proc/cpuinfo")
    if cpuinfo.exists():
        for line in cpuinfo.read_text(encoding="utf-8", errors="ignore").splitlines():
            if line.lower().startswith("model name"):
                return line.split(":", 1)[1].strip()
    return "Unknown"


def get_cpu_info():
    freq = psutil.cpu_freq()
    return {
        "model": _read_cpu_model(),
        "physical_cores": psutil.cpu_count(logical=False) or 0,
        "logical_cores": psutil.cpu_count(logical=True) or 0,
        "frequency_mhz": round(freq.current, 1) if freq else 0.0,
    }


def get_gpu_info():
    command = ["nvidia-smi", "--query-gpu=name,memory.total", "--format=csv,noheader,nounits"]
    try:
        output = subprocess.check_output(command, text=True, stderr=subprocess.DEVNULL).strip()
    except (FileNotFoundError, subprocess.CalledProcessError):
        return [{"name": "No NVIDIA GPU detected", "memory_gb": 0.0}]

    gpus = []
    for line in output.splitlines():
        parts = [p.strip() for p in line.split(",")]
        if len(parts) != 2:
            continue
        name, memory_mb = parts
        gpus.append({"name": name, "memory_gb": round(float(memory_mb) / 1024.0, 2)})
    return gpus or [{"name": "No NVIDIA GPU detected", "memory_gb": 0.0}]


def get_memory_info():
    memory = psutil.virtual_memory()
    return {
        "total_gb": round(memory.total / (1024**3), 2),
        "available_gb": round(memory.available / (1024**3), 2),
    }


def cpu_benchmark(duration_seconds=0.35):
    end_time = time.perf_counter() + duration_seconds
    value = 0
    iterations = 0
    while time.perf_counter() < end_time:
        value = (value * 1103515245 + 12345) & 0x7FFFFFFF
        iterations += 1
    return iterations / duration_seconds


def memory_benchmark(block_size=5_000_000):
    source = bytearray(b"x" * block_size)
    start = time.perf_counter()
    clone = source[:]
    elapsed = time.perf_counter() - start
    if clone[0] != source[0]:
        return 0.0
    throughput_mb_s = (block_size / (1024 * 1024)) / max(elapsed, 1e-6)
    return throughput_mb_s


def estimate_llm_tokens(cpu_info, gpu_info, memory_info, cpu_score, memory_score):
    gpu_memory = sum(gpu["memory_gb"] for gpu in gpu_info)
    weighted_score = (
        cpu_score * 0.40
        + memory_score * 75 * 0.15
        + cpu_info["logical_cores"] * 50 * 0.20
        + memory_info["total_gb"] * 30 * 0.10
        + gpu_memory * 140 * 0.15
    )
    yearly_tokens = int(max(weighted_score, 1) * 40_000)
    return yearly_tokens


def main():
    scenario = read_scenario("test.txt")
    cpu_info = get_cpu_info()
    gpu_info = get_gpu_info()
    memory_info = get_memory_info()
    cpu_score = cpu_benchmark()
    memory_score = memory_benchmark()

    llm_tokens = estimate_llm_tokens(cpu_info, gpu_info, memory_info, cpu_score, memory_score)

    print("Scenario from test.txt:")
    print(scenario)

    print("\nCPU:")
    for key, value in cpu_info.items():
        print(f"{key}: {value}")
    print(f"benchmark_ops_per_sec: {int(cpu_score):,}")

    print("\nGPU:")
    for gpu in gpu_info:
        print(f"name: {gpu['name']}")
        print(f"memory_gb: {gpu['memory_gb']}")

    print("\nMemory:")
    for key, value in memory_info.items():
        print(f"{key}: {value}")
    print(f"bandwidth_mb_per_sec: {memory_score:.2f}")

    print(f"\nEstimated LLM tokens per year: {llm_tokens:,}")


if __name__ == "__main__":
    main()
