#!/usr/bin/env python3
"""Walk-forward ETF/stock portfolio optimizer.

This is a more robust research companion to ETFModeling/ETFModeling.cpp.  It
uses the existing ETFModeling/marketData CSV files, optimizes only on trailing
training windows, then measures out-of-sample performance on future periods.

The implementation intentionally depends only on the Python standard library so
it can run in a fresh checkout after the repository Ubuntu setup script.
"""

from __future__ import annotations

import argparse
import csv
import math
import os
import random
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple

TRADING_DAYS_PER_YEAR = 252.0

DEFAULT_TICKERS = "SPY,QQQ,VTI,BND,AGG,GLD,VNQ,VEA,VWO"
DEFAULT_BENCHMARKS = "SPY,QQQ,VTI"


@dataclass
class MarketData:
    tickers: List[str]
    dates: List[str]
    prices: List[List[float]]
    returns: List[List[float]]
    return_dates: List[str]


@dataclass
class Metrics:
    total_return: float
    cagr: float
    volatility: float
    sharpe: float
    sortino: float
    max_drawdown: float
    calmar: float


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Walk-forward ETF portfolio optimizer")
    parser.add_argument("--data-folder", default="ETFModeling/marketData", help="folder with <TICKER>.csv files")
    parser.add_argument("--output-dir", default="ETFModeling2/output", help="directory for generated CSV reports")
    parser.add_argument("--tickers", default=DEFAULT_TICKERS, help="comma-separated investable tickers")
    parser.add_argument("--benchmarks", default=DEFAULT_BENCHMARKS, help="comma-separated benchmark tickers")
    parser.add_argument("--start-date", default="", help="optional inclusive YYYY-MM-DD start date")
    parser.add_argument("--end-date", default="", help="optional inclusive YYYY-MM-DD end date")
    parser.add_argument("--train-years", type=float, default=5.0, help="trailing years used for each optimization")
    parser.add_argument("--rebalance-months", type=int, default=1, help="months between rebalances")
    parser.add_argument("--max-weight", type=float, default=0.25, help="maximum weight per asset")
    parser.add_argument("--trials", type=int, default=1500, help="random optimizer candidates per rebalance")
    parser.add_argument("--refine-steps", type=int, default=35, help="coordinate-refinement rounds per rebalance")
    parser.add_argument("--risk-weight", type=float, default=0.70, help="downside volatility penalty")
    parser.add_argument("--vol-weight", type=float, default=0.15, help="total volatility penalty")
    parser.add_argument("--concentration-weight", type=float, default=0.10, help="sum(weight^2) penalty")
    parser.add_argument("--turnover-weight", type=float, default=0.02, help="optimization penalty versus previous weights")
    parser.add_argument("--transaction-cost-bps", type=float, default=1.0, help="one-way rebalance cost in basis points")
    parser.add_argument("--seed", type=int, default=1, help="deterministic optimizer seed")
    return parser.parse_args()


def split_csv_words(value: str) -> List[str]:
    return [x.strip().upper() for x in value.split(",") if x.strip()]


def read_ticker_csv(data_folder: Path, ticker: str) -> Dict[str, float]:
    path = data_folder / f"{ticker}.csv"
    if not path.exists():
        raise FileNotFoundError(f"missing market data for {ticker}: {path}")

    result: Dict[str, float] = {}
    with path.open(newline="") as f:
        reader = csv.DictReader(f)
        if "Date" not in (reader.fieldnames or []) or "Adj Close" not in (reader.fieldnames or []):
            raise ValueError(f"{path} must contain Date and Adj Close columns")
        for row in reader:
            date = row.get("Date", "")
            price_text = row.get("Adj Close", "")
            if not date or not price_text:
                continue
            price = float(price_text)
            if price > 0 and math.isfinite(price):
                result[date] = price
    if not result:
        raise ValueError(f"no valid prices loaded for {ticker}: {path}")
    return result


def load_market_data(
    data_folder: Path,
    tickers: Sequence[str],
    start_date: str = "",
    end_date: str = "",
) -> MarketData:
    raw = [read_ticker_csv(data_folder, ticker) for ticker in tickers]
    all_dates = sorted(set().union(*(set(x) for x in raw)))
    if start_date:
        all_dates = [d for d in all_dates if d >= start_date]
    if end_date:
        all_dates = [d for d in all_dates if d <= end_date]

    last_prices: List[float | None] = [None] * len(tickers)
    dates: List[str] = []
    prices: List[List[float]] = []
    for date in all_dates:
        for i, by_date in enumerate(raw):
            if date in by_date:
                last_prices[i] = by_date[date]
        if all(x is not None for x in last_prices):
            dates.append(date)
            prices.append([float(x) for x in last_prices])

    if len(prices) < 2:
        raise ValueError("not enough overlapping price history for selected tickers")

    returns: List[List[float]] = []
    return_dates: List[str] = []
    for i in range(1, len(prices)):
        returns.append([prices[i][j] / prices[i - 1][j] - 1.0 for j in range(len(tickers))])
        return_dates.append(dates[i])
    return MarketData(list(tickers), dates, prices, returns, return_dates)


def stdev(values: Sequence[float]) -> float:
    if len(values) < 2:
        return 0.0
    mean = sum(values) / len(values)
    variance = sum((x - mean) ** 2 for x in values) / (len(values) - 1)
    return math.sqrt(max(variance, 0.0))


def portfolio_returns(returns: Sequence[Sequence[float]], weights: Sequence[float]) -> List[float]:
    return [sum(w * row[i] for i, w in enumerate(weights)) for row in returns]


def project_weights(weights: Sequence[float], max_weight: float) -> List[float]:
    if max_weight <= 0:
        raise ValueError("max_weight must be positive")
    if max_weight * len(weights) < 1.0 - 1e-12:
        raise ValueError("max_weight is too small for the number of assets")

    clipped = [min(max(w, 0.0), max_weight) for w in weights]
    total = sum(clipped)
    if total <= 0:
        clipped = [1.0 / len(weights)] * len(weights)
    else:
        clipped = [w / total for w in clipped]

    # Repeatedly cap overweight assets and redistribute the remainder.
    for _ in range(len(weights) + 1):
        over = [i for i, w in enumerate(clipped) if w > max_weight]
        if not over:
            break
        fixed = set(over)
        fixed_sum = max_weight * len(fixed)
        free_sum = sum(clipped[i] for i in range(len(clipped)) if i not in fixed)
        remaining = max(1.0 - fixed_sum, 0.0)
        next_weights = [max_weight if i in fixed else 0.0 for i in range(len(clipped))]
        for i, w in enumerate(clipped):
            if i not in fixed:
                next_weights[i] = remaining * (w / free_sum) if free_sum > 0 else remaining / (len(clipped) - len(fixed))
        clipped = next_weights
    return clipped


def random_weights(n: int, max_weight: float, rng: random.Random) -> List[float]:
    return project_weights([rng.expovariate(1.0) for _ in range(n)], max_weight)


def objective(
    returns: Sequence[Sequence[float]],
    weights: Sequence[float],
    previous_weights: Sequence[float],
    risk_weight: float,
    vol_weight: float,
    concentration_weight: float,
    turnover_weight: float,
) -> float:
    p_returns = portfolio_returns(returns, weights)
    mean_daily = sum(p_returns) / len(p_returns)
    vol = stdev(p_returns) * math.sqrt(TRADING_DAYS_PER_YEAR)
    downside = stdev([min(x, 0.0) for x in p_returns]) * math.sqrt(TRADING_DAYS_PER_YEAR)
    annual_return = mean_daily * TRADING_DAYS_PER_YEAR
    concentration = sum(w * w for w in weights)
    turnover = sum(abs(weights[i] - previous_weights[i]) for i in range(len(weights)))
    return annual_return - risk_weight * downside - vol_weight * vol - concentration_weight * concentration - turnover_weight * turnover


def optimize_weights(
    returns: Sequence[Sequence[float]],
    previous_weights: Sequence[float],
    args: argparse.Namespace,
    rng: random.Random,
) -> List[float]:
    n = len(previous_weights)
    candidates: List[List[float]] = [project_weights(previous_weights, args.max_weight), [1.0 / n] * n]
    candidates.extend(random_weights(n, args.max_weight, rng) for _ in range(args.trials))

    def score(w: Sequence[float]) -> float:
        return objective(
            returns,
            w,
            previous_weights,
            args.risk_weight,
            args.vol_weight,
            args.concentration_weight,
            args.turnover_weight,
        )

    best = max(candidates, key=score)
    best_score = score(best)

    step = 0.05
    for _ in range(args.refine_steps):
        improved = False
        for i in range(n):
            for direction in (-1.0, 1.0):
                trial = list(best)
                trial[i] += direction * step
                trial = project_weights(trial, args.max_weight)
                trial_score = score(trial)
                if trial_score > best_score:
                    best, best_score, improved = trial, trial_score, True
        if not improved:
            step *= 0.55
            if step < 1e-4:
                break
    return best


def month_key(date: str) -> Tuple[int, int]:
    return int(date[:4]), int(date[5:7])


def month_distance(a: str, b: str) -> int:
    ay, am = month_key(a)
    by, bm = month_key(b)
    return (by - ay) * 12 + (bm - am)


def rebalance_indices(dates: Sequence[str], rebalance_months: int) -> List[int]:
    result = [0]
    last = dates[0]
    for i, date in enumerate(dates[1:], 1):
        if month_distance(last, date) >= rebalance_months:
            result.append(i)
            last = date
    if result[-1] != len(dates) - 1:
        result.append(len(dates) - 1)
    return result


def compute_metrics(equity: Sequence[float], daily_returns: Sequence[float]) -> Metrics:
    if not daily_returns:
        return Metrics(0, 0, 0, 0, 0, 0, 0)
    growth = equity[-1] / equity[0]
    total_return = growth - 1.0
    years = len(daily_returns) / TRADING_DAYS_PER_YEAR
    cagr = growth ** (1.0 / years) - 1.0 if years > 0 and growth > 0 else 0.0
    daily_std = stdev(daily_returns)
    vol = daily_std * math.sqrt(TRADING_DAYS_PER_YEAR)
    mean = sum(daily_returns) / len(daily_returns)
    sharpe = mean / daily_std * math.sqrt(TRADING_DAYS_PER_YEAR) if daily_std > 0 else 0.0
    downside_std = stdev([min(x, 0.0) for x in daily_returns])
    sortino = mean / downside_std * math.sqrt(TRADING_DAYS_PER_YEAR) if downside_std > 0 else 0.0
    peak = equity[0]
    max_dd = 0.0
    for value in equity:
        peak = max(peak, value)
        max_dd = min(max_dd, value / peak - 1.0)
    calmar = cagr / abs(max_dd) if max_dd < 0 else 0.0
    return Metrics(total_return, cagr, vol, sharpe, sortino, max_dd, calmar)


def run_backtest(md: MarketData, args: argparse.Namespace) -> Tuple[List[Dict[str, str]], List[Dict[str, str]], Dict[str, Metrics]]:
    rng = random.Random(args.seed)
    n = len(md.tickers)
    train_days = max(2, int(args.train_years * TRADING_DAYS_PER_YEAR))
    rebalances = rebalance_indices(md.return_dates, args.rebalance_months)
    rebalances = [i for i in rebalances if i >= train_days]
    if len(rebalances) < 2:
        raise ValueError("not enough history after train window; lower --train-years or add more data")

    weights = [1.0 / n] * n
    equity = 1.0
    equity_rows: List[Dict[str, str]] = []
    allocation_rows: List[Dict[str, str]] = []
    strategy_returns: List[float] = []

    benchmark_tickers = [t for t in split_csv_words(args.benchmarks) if t in md.tickers]
    benchmark_equity = {ticker: 1.0 for ticker in benchmark_tickers}
    benchmark_returns = {ticker: [] for ticker in benchmark_tickers}
    equal_equity = 1.0
    equal_returns: List[float] = []

    print("run_backtest 1...")
    for rb_pos, start in enumerate(rebalances[:-1]):
        print("     run_backtest 2 " + str(len(rebalances)) + "...")
        end = rebalances[rb_pos + 1]
        train = md.returns[start - train_days : start]
        new_weights = optimize_weights(train, weights, args, rng)
        turnover = sum(abs(new_weights[i] - weights[i]) for i in range(n))
        cost = args.transaction_cost_bps / 10000.0 * turnover
        equity *= max(0.0, 1.0 - cost)
        weights = new_weights

        row = {"date": md.return_dates[start], "turnover": f"{turnover:.8f}", "cost": f"{cost:.8f}"}
        row.update({ticker: f"{weights[i]:.8f}" for i, ticker in enumerate(md.tickers)})
        allocation_rows.append(row)

        for day in range(start, end):
            r = sum(weights[i] * md.returns[day][i] for i in range(n))
            equity *= 1.0 + r
            strategy_returns.append(r)

            equal_r = sum(md.returns[day]) / n
            equal_equity *= 1.0 + equal_r
            equal_returns.append(equal_r)

            for ticker in benchmark_tickers:
                idx = md.tickers.index(ticker)
                br = md.returns[day][idx]
                benchmark_equity[ticker] *= 1.0 + br
                benchmark_returns[ticker].append(br)

            equity_rows.append({
                "date": md.return_dates[day],
                "strategy": f"{equity:.8f}",
                "equal_weight": f"{equal_equity:.8f}",
                **{ticker: f"{benchmark_equity[ticker]:.8f}" for ticker in benchmark_tickers},
            })

    metrics: Dict[str, Metrics] = {
        "strategy": compute_metrics([float(r["strategy"]) for r in equity_rows], strategy_returns),
        "equal_weight": compute_metrics([float(r["equal_weight"]) for r in equity_rows], equal_returns),
    }
    for ticker in benchmark_tickers:
        metrics[ticker] = compute_metrics([float(r[ticker]) for r in equity_rows], benchmark_returns[ticker])
    return equity_rows, allocation_rows, metrics


def write_csv(path: Path, rows: Sequence[Dict[str, str]]) -> None:
    if not rows:
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)


def write_metrics(path: Path, metrics: Dict[str, Metrics]) -> None:
    rows = []
    for name, m in metrics.items():
        rows.append({
            "name": name,
            "total_return": f"{m.total_return:.8f}",
            "cagr": f"{m.cagr:.8f}",
            "volatility": f"{m.volatility:.8f}",
            "sharpe": f"{m.sharpe:.8f}",
            "sortino": f"{m.sortino:.8f}",
            "max_drawdown": f"{m.max_drawdown:.8f}",
            "calmar": f"{m.calmar:.8f}",
        })
    write_csv(path, rows)


def main() -> int:
    args = parse_args()
    tickers = split_csv_words(args.tickers)
    os.makedirs(args.output_dir, exist_ok=True)
    md = load_market_data(Path(args.data_folder), tickers, args.start_date, args.end_date)
    print("MD is ready")
    equity_rows, allocation_rows, metrics = run_backtest(md, args)

    output_dir = Path(args.output_dir)
    write_csv(output_dir / "equity_curve.csv", equity_rows)
    write_csv(output_dir / "allocations.csv", allocation_rows)
    write_metrics(output_dir / "metrics.csv", metrics)

    print(f"Loaded {len(md.tickers)} tickers and {len(md.return_dates)} daily returns")
    print(f"Wrote {output_dir / 'equity_curve.csv'}")
    print(f"Wrote {output_dir / 'allocations.csv'}")
    print(f"Wrote {output_dir / 'metrics.csv'}")
    for name, m in metrics.items():
        print(
            f"{name:>12}: CAGR={m.cagr:8.2%} Sharpe={m.sharpe:6.2f} "
            f"Sortino={m.sortino:6.2f} MaxDD={m.max_drawdown:8.2%}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
