#!/usr/bin/env python
#!/usr/bin/env python3

import platform
import shutil
import subprocess
import time
from pathlib import Path

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


def ollama_available():
    if not shutil.which("ollama"):
        return False
    try:
        subprocess.check_output(["ollama", "list"], text=True, stderr=subprocess.DEVNULL)
        return True
    except subprocess.CalledProcessError:
        return False


def estimate_tokens_per_year(cpu, memory, gpus, cpu_ops, mem_mb_s, has_ollama):
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
    return int(max(base, 1) * 35_000)


def main():
    spec = read_spec("test2.txt")
    system = get_system_info()
    cpu = get_cpu_info()
    memory = get_memory_info()
    gpus = detect_gpu()
    cpu_ops = cpu_benchmark()
    mem_bw = memory_benchmark()
    has_ollama = ollama_available()
    tokens = estimate_tokens_per_year(cpu, memory, gpus, cpu_ops, mem_bw, has_ollama)

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
    print(f"\nOllama available: {has_ollama}")
    print(f"Estimated LLM tokens per year: {tokens:,}")


if __name__ == "__main__":
    main()
