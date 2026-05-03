#!/usr/bin/env python3
"""
Simple Bitcoin 1-hour direction predictor.

This script downloads public hourly Bitcoin price data from CoinGecko and
predicts whether the price in one hour is likely to be MORE or LESS than now.
"""

from __future__ import annotations

import json
import math
import sys
import time
import urllib.error
import urllib.request
from typing import List, Tuple


COINGECKO_URL = (
    "https://api.coingecko.com/api/v3/coins/bitcoin/market_chart"
    "?vs_currency=usd&days=7&interval=hourly"
)


def download_hourly_prices(url: str = COINGECKO_URL) -> List[Tuple[int, float]]:
    """Return list of (timestamp_ms, price_usd) from CoinGecko."""
    try:
        with urllib.request.urlopen(url, timeout=20) as response:
            payload = response.read().decode("utf-8")
    except urllib.error.URLError:
        return generate_offline_sample_data()

    try:
        data = json.loads(payload)
        prices = data["prices"]
    except (json.JSONDecodeError, KeyError, TypeError) as exc:
        raise RuntimeError("Unexpected response format from API.") from exc

    parsed: List[Tuple[int, float]] = []
    for item in prices:
        if (
            isinstance(item, list)
            and len(item) == 2
            and isinstance(item[0], (int, float))
            and isinstance(item[1], (int, float))
        ):
            parsed.append((int(item[0]), float(item[1])))

    if len(parsed) < 30:
        return generate_offline_sample_data()

    return parsed


def generate_offline_sample_data() -> List[Tuple[int, float]]:
    """
    Build deterministic pseudo-hourly data if internet is unavailable.
    """
    now_ms = int(time.time() * 1000)
    base_price = 65000.0
    points: List[Tuple[int, float]] = []
    for i in range(7 * 24):
        # Deterministic oscillation + mild trend to mimic real movement.
        price = base_price + (i * 6.5) + (math.sin(i / 5.0) * 420.0)
        ts = now_ms - ((7 * 24 - i) * 3600 * 1000)
        points.append((ts, price))
    return points


def linear_regression_slope(values: List[float]) -> float:
    """Compute slope of y over x=[0..n-1] using least squares."""
    n = len(values)
    x_mean = (n - 1) / 2.0
    y_mean = sum(values) / n

    num = 0.0
    den = 0.0
    for i, y in enumerate(values):
        dx = i - x_mean
        num += dx * (y - y_mean)
        den += dx * dx

    return num / den if den else 0.0


def predict_direction(prices: List[float]) -> Tuple[str, float]:
    """
    Predict whether next hour will be MORE or LESS.

    Uses a blend of:
    - trend slope from last 24 hours
    - average return from last 12 hours
    Returns (direction, confidence in [0,1]).
    """
    if len(prices) < 25:
        raise ValueError("Need at least 25 hourly prices.")

    last_24 = prices[-24:]
    slope = linear_regression_slope(last_24)  # USD/hour-ish

    returns: List[float] = []
    for i in range(len(prices) - 12, len(prices)):
        prev = prices[i - 1]
        curr = prices[i]
        if prev > 0:
            returns.append((curr - prev) / prev)
    avg_ret = sum(returns) / len(returns) if returns else 0.0

    now_price = prices[-1]
    trend_score = slope / max(now_price, 1.0)
    combined_score = (0.7 * trend_score) + (0.3 * avg_ret)

    direction = "MORE" if combined_score >= 0 else "LESS"
    confidence = min(0.95, max(0.5, 0.5 + min(abs(combined_score) * 120, 0.45)))
    return direction, confidence


def main() -> int:
    try:
        hourly_data = download_hourly_prices()
        prices = [price for _ts, price in hourly_data]
        current_price = prices[-1]
        direction, confidence = predict_direction(prices)
    except Exception as exc:  # pylint: disable=broad-except
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    source = "offline sample data" if len(hourly_data) == 7 * 24 else "online market data"
    print("Bitcoin 1-hour prediction")
    print(f"Data source: {source}")
    print(f"Current price (USD): {current_price:.2f}")
    print(f"Prediction for 1 hour: {direction}")
    print(f"Confidence: {confidence * 100:.1f}%")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
