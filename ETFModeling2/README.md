# ETFModeling2

A walk-forward portfolio research tool built on the existing `ETFModeling/marketData/*.csv` files.

The original `ETFModeling` C++ binary optimizes one portfolio on the full historical period. That is useful for exploration, but it can overfit because the same data is used both to choose and to judge the allocation.

`ETFModeling2/portfolio_model.py` keeps the same general idea — choose ETF/stock weights from historical price data — but makes the experiment more realistic:

- optimizes on a trailing training window only;
- evaluates the chosen weights out-of-sample until the next rebalance;
- repeats this walk-forward through time;
- penalizes downside risk, total volatility, concentration, and turnover;
- enforces long-only weights and a maximum weight per asset;
- charges a simple transaction-cost estimate at each rebalance;
- writes benchmarkable CSV reports.

The script intentionally uses only the Python standard library.

## Quick start

From the repo root:

```sh
python3 ETFModeling2/portfolio_model.py
```

Default universe:

```text
SPY, QQQ, VTI, BND, AGG, GLD, VNQ, VEA, VWO
```

Default outputs are written under `ETFModeling2/output/`:

- `equity_curve.csv` — strategy, equal-weight, and benchmark equity curves;
- `allocations.csv` — optimized weights at each rebalance;
- `metrics.csv` — CAGR, volatility, Sharpe, Sortino, max drawdown, Calmar.

## Example runs

Monthly walk-forward using five years of trailing data:

```sh
python3 ETFModeling2/portfolio_model.py \
  --train-years 5 \
  --rebalance-months 1 \
  --max-weight 0.25
```

Include a larger ticker universe and compare against benchmarks:

```sh
python3 ETFModeling2/portfolio_model.py \
  --tickers SPY,QQQ,VTI,BND,AGG,GLD,VNQ,VEA,VWO,AAPL,MSFT,NVDA \
  --benchmarks SPY,QQQ,VTI \
  --max-weight 0.20 \
  --transaction-cost-bps 2
```

Use a shorter, faster optimization for smoke testing:

```sh
python3 ETFModeling2/portfolio_model.py \
  --tickers SPY,QQQ,BND,GLD \
  --train-years 3 \
  --trials 100 \
  --refine-steps 5 \
  --output-dir /tmp/etf-modeling2-smoke
```

## Objective

For each rebalance, the optimizer samples candidate portfolios and locally refines the best one. It maximizes roughly:

```text
annualized mean return
- risk_weight * annualized downside volatility
- vol_weight * annualized volatility
- concentration_weight * sum(weight_i^2)
- turnover_weight * sum(abs(weight_i - previous_weight_i))
```

Then the chosen allocation is held out-of-sample until the next rebalance.

## Notes and limitations

- Uses `Adj Close` as a total-return proxy. The original C++ model also reads `Dividends`; using adjusted close avoids double-counting dividends from Yahoo-style data.
- The optimizer is stochastic but deterministic for a fixed `--seed`.
- This is still research code, not financial advice. Results depend heavily on the ticker universe, date range, costs, and constraints.
- The goal is to make overfitting more visible, not to prove a live trading strategy.
