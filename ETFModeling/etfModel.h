#pragma once

#include "gflags/gflags.h"

#include "lib/header.h"
#include "lib/stat.h"

DECLARE_string(first_date);
DECLARE_bool(additive_sortino);
DECLARE_int32(iterations);
DECLARE_double(risk_weight);
DECLARE_bool(decay);
DECLARE_double(concentration_risk_weight);
DECLARE_string(dataFolder);

struct PriceData {
    StringVector tickers_;
    vector<DoubleVector> prices_;
    vector<DoubleVector> dividends_;
    vector<string> dates_;

    PriceData subPriceData(const SizeTVector& indices);
};

// Reads FLAGS_dataFolder + "marketData/<ticker>.csv" for every ticker. Column 0
// of the result is the cash position "CORE**" with a constant price of 1.
PriceData loadData(const StringVector& tickers);

using Portfolio = DoubleVector;

void normalizeNavInplace(Portfolio& p);

struct ModelResult {
    Portfolio originalNav;
    Portfolio originalShares;
    Portfolio finalNav;
    Stat<> returnsStat;
    double sharpe{};
    double dailySharpe{};
    DoubleVector dailyPrices;
    DoubleVector dailyReturns;
    Stat<> dailyReturnsStat;
    Stat<> dailyNegReturnsStat;
    double sortino{};
    double additiveSortino{};
    double concentration{};
    double f{-1e10};
    double dividends{};
    string lastDate{};

    void calcReturns();
};

double sharpe(const ModelResult& res);
ModelResult model(const PriceData& pd, const Portfolio& originalNav, bool useConcentration);
void out(const ModelResult& res);
Portfolio randomPortfolio(const StringVector& tickers);
Portfolio loadPortfolio(const PriceData& pd, const string& filename);
ModelResult gradientSearch(const PriceData& pd);
void dumpPricesToCsv(const PriceData& pd, const ModelResult& model, const string& filename);

struct StrategyResult {
    ModelResult buyAndHold;
    ModelResult minMax;
};

// Buys at every local minimum and sells at every local maximum of the price of
// ticker i, compared with holding it throughout.
StrategyResult minMaxStrategy(const PriceData& pd, size_t i);
