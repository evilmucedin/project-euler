#include <set>

#include "gflags/gflags.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/ml/dataframe.h"
#include "lib/random.h"
#include "lib/stat.h"
#include "lib/timer.h"

DEFINE_string(first_date, "", "First modeling date");
DEFINE_bool(additive_sortino, false, "subtract risk");
DEFINE_int32(iterations, 10, "nuimber of iterations");
DEFINE_string(mode, "optimize", "mode (optimize, optimize1)");

static const StringVector etfs = {"FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "IWM",  "VUG",  "SPY",
                                  "IVV",   "VOO",   "QQQ",   "BND",   "FBND",  "HDV",   "VEU",  "VWO",  "FDHY",
                                  "FDIS",  "ONEQ",  "VV",    "VB",    "HNDL",  "WBII",  "PCEF", "FDIV", "CEFS",
                                  "YLD",   "INKM",  "IYLD",  "FCEF",  "MLTI",  "YYY",   "MDIV", "HIPS", "CVY",
                                  "GYLD",  "VTI",   "VEA",   "IEFA",  "AGG",   "GLD",   "XLF",  "VNQ",  "LQD", "SWPPX"};

static const StringVector stocks = {
    "GOOG", "MSFT", "T", "NCLH", "OGZPY", "AMZN", "FB", "TSLA",
};

// static const StringVector tickers = etfs;

/*
 */

// static const StringVector tickers = {"TSLA"};

struct PriceData {
    StringVector tickers_;
    vector<DoubleVector> prices_;
    vector<string> dates_;

    PriceData subPriceData(const SizeTVector& indices) {
        PriceData result;
        result.dates_ = dates_;
        for (auto i : indices) {
            result.tickers_.emplace_back(tickers_[i]);
        }
        for (const auto& v : prices_) {
            result.prices_.emplace_back(subVector(v, indices));
        }
        return result;
    }
};

PriceData loadData(const StringVector& tickers) {
    Timer tSolve("Load data");

    using Date2Price = unordered_map<string, double>;
    vector<Date2Price> mappedData;
    set<string> dates;
    {
        Timer tFileRead("Read files");
        for (const auto& ticker : tickers) {
            auto df = DataFrame::loadFromCsv(ticker + ".csv");
            auto date = df->getColumn("Date");
            auto price = df->getColumn("Adj Close");
            Date2Price tickerPrices;
            for (size_t i = 0; i < df->numLines(); ++i) {
                const auto iDate = date->as<string>(i);
                if (FLAGS_first_date.empty() || (iDate >= FLAGS_first_date)) {
                    tickerPrices[iDate] = price->as<double>(i);
                    dates.emplace(iDate);
                }
            }
            mappedData.emplace_back(std::move(tickerPrices));
        }
    }

    PriceData result;
    result.tickers_ = tickers;
    result.prices_.resize(dates.size());
    for (auto& col : result.prices_) {
        col.resize(tickers.size());
    }
    size_t iDate = 0;
    for (const auto& date : dates) {
        for (size_t i = 0; i < tickers.size(); ++i) {
            const auto toDate = mappedData[i].find(date);
            if (toDate != mappedData[i].end()) {
                result.prices_[iDate][i] = toDate->second;
            } else {
                if (iDate) {
                    result.prices_[iDate][i] = result.prices_[iDate - 1][i];
                }
            }
        }
        ++iDate;
    }

    for (ssize_t i = dates.size() - 2; i >= 0; --i) {
        for (size_t j = 0; j < tickers.size(); ++j) {
            if (!result.prices_[i][j]) {
                result.prices_[i][j] = result.prices_[i + 1][j];
            }
            ALWAYS_ASSERT(isfinite(result.prices_[i][j]));
        }
    }

    result.dates_.insert(result.dates_.end(), dates.begin(), dates.end());

    return result;
}

using Portfolio = DoubleVector;

void normalizeNavInplace(Portfolio& p) {
    double sm = sum(p);
    ASSERTNEQ(sm, 0);
    for (auto& x : p) {
        x /= sm;
    }
}

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
    double f{-1e10};
};

double sharpe(const ModelResult& res) {
    return (sum(res.finalNav) - sum(res.originalNav)) / sum(res.originalNav) / res.returnsStat.stddev();
    // return (sum(res.finalNav) - sum(res.originalNav)) / sum(res.originalNav) - 5 * res.returnsStat.stddev();
}

ModelResult model(const PriceData& pd, const Portfolio& originalNav) {
    ModelResult result;
    result.originalNav = originalNav;
    result.originalShares.resize(pd.tickers_.size());
    for (size_t i = 0; i < pd.tickers_.size(); ++i) {
        result.originalShares[i] = result.originalNav[i] / pd.prices_.front()[i];
        ALWAYS_ASSERT(isfinite(result.originalShares[i]));
    }
    const double originalNavSum = sum(originalNav);

    result.dailyPrices.reserve(pd.prices_.size());
    result.dailyReturns.reserve(pd.prices_.size());
    for (size_t i = 0; i < pd.prices_.size(); ++i) {
        double nav = 0.0;
        for (size_t j = 0; j < pd.tickers_.size(); ++j) {
            nav += pd.prices_[i][j] * result.originalShares[j];
        }
        result.dailyPrices.emplace_back(nav);
        double ret = (nav - originalNavSum) / originalNavSum;
        result.returnsStat.add(ret);
    }

    for (size_t i = 1; i < result.dailyPrices.size(); ++i) {
        const double dailyRet = log(result.dailyPrices[i] / result.dailyPrices[i - 1]);
        ALWAYS_ASSERT(isfinite(dailyRet));
        result.dailyReturns.emplace_back(dailyRet);
        result.dailyReturnsStat.add(dailyRet);
        if (dailyRet < 0) {
            result.dailyNegReturnsStat.add(dailyRet);
        } else {
            result.dailyNegReturnsStat.add(0.0);
        }
    }

    result.sortino = result.dailyReturnsStat.mean() / result.dailyNegReturnsStat.stddev();
    result.additiveSortino = result.dailyReturnsStat.mean() - 0.05 * result.dailyNegReturnsStat.stddev();

    result.finalNav.resize(pd.tickers_.size());
    for (size_t i = 0; i < pd.tickers_.size(); ++i) {
        result.finalNav[i] = result.originalShares[i] * pd.prices_.back()[i];
    }

    result.sharpe = sharpe(result);
    result.dailySharpe = result.dailyReturnsStat.mean() / result.dailyReturnsStat.stddev();

    if (FLAGS_additive_sortino) {
        result.f = result.additiveSortino;
    } else {
        result.f = result.sortino;
    }

    return result;
}

void out(const ModelResult& res) {
    const auto before = sum(res.originalNav);
    const auto after = sum(res.finalNav);
    cout << res.returnsStat << ", initial NAV: " << before << ", final NAV: " << after << ", sharpe: " << res.sharpe
         << ", sortino: " << res.sortino << ", f: " << res.f << endl;
}

void testModeling(const PriceData& pd) {
    cout << "Init prices: " << pd.prices_.front() << endl;
    cout << "Final prices: " << pd.prices_.back() << endl;
    Portfolio p(pd.tickers_.size(), 1);
    normalizeNavInplace(p);
    auto res = model(pd, p);
    out(res);
}

Portfolio randomPortfolio(const StringVector& tickers) {
    Portfolio result(tickers.size());
    for (auto& x : result) {
        x = rand();
    }
    normalizeNavInplace(result);
    return result;
}

ModelResult gradientSearch(const PriceData& pd) {
    Timer tSolve("Gradient search");

    ModelResult bestRes;
    bestRes.originalNav.resize(pd.tickers_.size(), 1);
    normalizeNavInplace(bestRes.originalNav);

    auto eval = [&](Portfolio& c) {
        if (sum(c) != 0) {
            normalizeNavInplace(c);
            const auto res = model(pd, c);
            if (res.f > bestRes.f) {
                bestRes = res;
            }
        }
    };

    for (size_t k = 0; k < FLAGS_iterations; ++k) {
        ModelResult bestRes1;
        bestRes1.originalNav = randomPortfolio(pd.tickers_);

        auto eval1 = [&](Portfolio& c) {
            normalizeNavInplace(c);
            const auto res = model(pd, c);
            if (res.f > bestRes1.f) {
                bestRes1 = res;
            }
        };

        for (size_t i = 0; i < 200; ++i) {
            double scale = (i & 1) ? 0.005 : 0.05;
            for (size_t j = 0; j < pd.tickers_.size(); ++j) {
                Portfolio c = bestRes1.originalNav;
                c[j] *= 1.0 - scale;
                eval1(c);
                c = bestRes1.originalNav;
                c[j] *= 1.0 + scale;
                eval1(c);
            }
            cerr << k << " " << i << " " << bestRes1.f << " " << bestRes1.originalNav << endl;
        }

        eval(bestRes1.originalNav);
    }

    for (size_t j = 0; j < pd.tickers_.size(); ++j) {
        auto c = bestRes.originalNav;
        c[j] = 0;
        eval(c);
    }

    for (size_t i = 0; i < pd.tickers_.size(); ++i) {
        cout << pd.tickers_[i] << " " << bestRes.originalNav[i] << " " << bestRes.finalNav[i] << endl;
    }
    out(bestRes);

    return bestRes;
}

void dumpPricesToCsv(const PriceData &pd, const ModelResult &model,
                     const string &filename) {
  Timer tSolve("Gen CSV");

  DataFrame df(cat(StringVector{"Date", "Optimal"}, pd.tickers_));

  vector<DoubleVector> prices(pd.dates_.size(),
                              DoubleVector(1 + pd.tickers_.size()));
  for (size_t i = 0; i < pd.dates_.size(); ++i) {
    for (size_t j = 0; j < pd.tickers_.size(); ++j) {
      ALWAYS_ASSERT(isfinite(model.originalShares[j]));
      prices[i][0] += model.originalShares[j] * pd.prices_[i][j];
    }
  }
  for (size_t i = 0; i < pd.dates_.size(); ++i) {
    for (size_t j = 0; j < pd.tickers_.size(); ++j) {
      prices[i][j + 1] = pd.prices_[i][j];
    }
  }

  df.resizeLines(pd.dates_.size());

  for (size_t i = 0; i < pd.dates_.size(); ++i) {
    df.columns_[0]->set(i, pd.dates_[i]);
  }

  for (size_t i = 0; i < prices.size(); ++i) {
    for (size_t j = 0; j < prices[i].size(); ++j) {
      double norm = prices[i][j] / prices.front()[j];
      ALWAYS_ASSERT(isfinite(norm));
      df.columns_[j + 1]->set(i, norm);
    }
  }

  df.saveToCsv(filename);
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);


    if (FLAGS_mode == "optimize") {
      const StringVector tickers = etfs;
      auto data = loadData(tickers);
      // testModeling(data);
      auto best = gradientSearch(data);
      dumpPricesToCsv(data, best, "optimalAll.csv");

      SizeTVector nonZeroIndices;
      auto nonZeroBest = best;
      size_t index = 0;
      for (size_t i = 0; i < tickers.size(); ++i) {
        if (best.originalShares[i]) {
          nonZeroBest.originalShares[index++] = best.originalShares[i];
          nonZeroIndices.emplace_back(i);
        }
      }
      dumpPricesToCsv(data.subPriceData(nonZeroIndices), nonZeroBest,
                      "optimalNZ.csv");
    } else if (FLAGS_mode == "optimize1") {
      const StringVector tickers = cat(etfs, stocks);
      auto data = loadData(tickers);
    } else {
      THROW("Unknown mode '" << FLAGS_mode << "'");
    }

    return 0;
}
