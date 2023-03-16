#include <set>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "lib/header.h"
#include "lib/init.h"
#include "lib/ml/dataframe.h"
#include "lib/random.h"
#include "lib/stat.h"
#include "lib/thread-pool/threadPool.h"
#include "lib/timer.h"

DEFINE_string(first_date, "", "First modeling date");
DEFINE_bool(additive_sortino, true, "subtract risk");
DEFINE_int32(iterations, 10, "nuimber of iterations");
DEFINE_string(mode, "optimize", "mode (optimize, optimize1)");
DEFINE_string(input, "portfolio input", "");
DEFINE_bool(stocks, false, "add stocks");
DEFINE_double(risk_weight, 0.05, "risk weight");
DEFINE_bool(decay, true, "decay");
DEFINE_double(concentration_risk_weight, 0.00, "concentration risk weight");

static const StringVector etfs = {
    "FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "IWM",  "VUG",  "SPY",  "IVV",   "VOO",  "QQQ",
    "BND",   "FBND",  "HDV",   "VEU",   "VWO",   "FDHY",  "FDIS", "ONEQ", "VV",   "VB",    "HNDL", "WBII",
    "PCEF",  "FDIV",  "CEFS",  "YLD",   "INKM",  "IYLD",  "FCEF", "MLTI", "YYY",  "MDIV",  "HIPS", "CVY",
    "GYLD",  "VTI",   "VEA",   "IEFA",  "AGG",   "GLD",   "XLF",  "VNQ",  "LQD",  "SWPPX", "MGK",  "UNG",
    "OIH",   "XME",   "PFIX",  "VXX",   "EWZ",   "ILF",   "SCHE", "FBCG", "FQAL", "FLPSX", "FDRR", "FMAG",
    "FPRO",  "FBCV",  "FMIL",  "BITO",  "BITW",  "VBB",   "SFY",  "IJR",  "SCHD", "FTLS",  "FDHT", "FRNW",
    "FDRV",  "FCLD",  "FDIG",  "ARKK",  "ITA",   "PPA",   "XAR",  "USO",  "IEO",  "SPGP",  "IWY",  "SPYG",
};

static const StringVector stocks = {
    "GOOG", "MSFT", "T",   "NCLH", "AMZN", "META", "TSLA", "GME", "AAPL",  "NVDA", "TSM",  "UNH", "JNJ", "V",   "WMT",
    "JPM",  "PG",   "XOM", "HD",   "CVX",  "PFE",  "COIN", "SI",  "VWAGY", "LMT",  "YNDX", "KHC", "NKE", "SBUX",
};

// static const StringVector tickers = etfs;

/*
 */

// static const StringVector tickers = {"TSLA"};

struct PriceData {
    StringVector tickers_;
    vector<DoubleVector> prices_;
    vector<DoubleVector> dividends_;
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
        for (const auto& v : dividends_) {
            result.dividends_.emplace_back(subVector(v, indices));
        }
        return result;
    }
};

PriceData loadData(const StringVector& tickers) {
    Timer tSolve("Load data");

    using Date2Price = unordered_map<string, double>;
    vector<Date2Price> mappedPricesData;
    vector<Date2Price> mappedDividendsData;
    set<string> dates;
    {
        Timer tFileRead("Read files");
        for (const auto& ticker : tickers) {
            DLOG(INFO) << "Ticker: " << ticker;
            auto df = DataFrame::loadFromCsv("marketData/" + ticker + ".csv");
            auto date = df->getColumn("Date");
            auto price = df->getColumn("Adj Close");
            auto dividends = df->getColumn("Dividends");
            Date2Price tickerPrices;
            Date2Price tickerDividends;
            for (size_t i = 0; i < df->numLines(); ++i) {
                if (i >= date->size()) {
                    THROW("No date in '" << ticker << "' index: " << i);
                }
                const auto iDate = date->as<string>(i);
                if (FLAGS_first_date.empty() || (iDate >= FLAGS_first_date)) {
                    if (i >= price->size()) {
                        THROW("No price in '" << ticker << "' index: " << i);
                    }
                    tickerPrices[iDate] = price->as<double>(i);
                    ASSERTGT(tickerPrices[iDate], 0.0);
                    tickerDividends[iDate] = dividends->as<double>(i);
                    ASSERTGE(tickerDividends[iDate], 0.0);
                    dates.emplace(iDate);
                }
            }
            mappedPricesData.emplace_back(std::move(tickerPrices));
            mappedDividendsData.emplace_back(std::move(tickerDividends));
        }
    }

    PriceData result;
    result.tickers_ = cat(StringVector{"CORE**"}, tickers);
    result.prices_.resize(dates.size());
    result.dividends_.resize(dates.size());
    for (auto& col : result.prices_) {
        col.resize(result.tickers_.size());
    }
    for (auto& col : result.dividends_) {
        col.resize(result.dividends_.size());
    }
    size_t iDate = 0;
    for (const auto& date : dates) {
        result.prices_[iDate][0] = 1.0;
        for (size_t i = 1; i < result.tickers_.size(); ++i) {
            const auto toDate = mappedPricesData[i - 1].find(date);
            if (toDate != mappedPricesData[i - 1].end()) {
                result.prices_[iDate][i] = toDate->second;
            } else {
                if (iDate) {
                    result.prices_[iDate][i] = result.prices_[iDate - 1][i];
                }
            }
            const auto toDateD = mappedDividendsData[i - 1].find(date);
            if (toDateD != mappedDividendsData[i - 1].end()) {
                result.dividends_[iDate][i] = toDateD->second;
            }
        }
        ++iDate;
    }

    for (ssize_t i = dates.size() - 2; i >= 0; --i) {
        for (size_t j = 0; j < result.tickers_.size(); ++j) {
            if (!result.prices_[i][j]) {
                result.prices_[i][j] = result.prices_[i + 1][j];
            }
            ALWAYS_ASSERT(isfinite(result.prices_[i][j]));
            ASSERTNE(result.prices_[i][j], 0.0);
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
    double concentration{};
    double f{-1e10};
    double dividends{};
    string lastDate{};
};

double sharpe(const ModelResult& res) {
    return (sum(res.finalNav) - sum(res.originalNav)) / sum(res.originalNav) / res.returnsStat.stddev();
    // return (sum(res.finalNav) - sum(res.originalNav)) / sum(res.originalNav) - 5 * res.returnsStat.stddev();
}

ModelResult model(const PriceData& pd, const Portfolio& originalNav, bool useConcentration) {
    ASSERTEQ(pd.tickers_.size(), originalNav.size());
    ModelResult result;
    result.originalNav = originalNav;
    result.originalShares.resize(pd.tickers_.size());
    for (size_t i = 0; i < pd.tickers_.size(); ++i) {
        result.originalShares[i] = result.originalNav[i] / pd.prices_.front()[i];
        ALWAYS_ASSERT(isfinite(result.originalShares[i]));
        ALWAYS_ASSERT(result.originalShares[i] >= 0);
    }
    const double originalNavSum = sum(originalNav);

    static constexpr double DECAY_SPEED = 0.999;

    result.dailyPrices.reserve(pd.prices_.size());
    result.dailyReturns.reserve(pd.prices_.size());
    double dividendsSoFar = 0.0;
    for (size_t i = 0; i < pd.prices_.size(); ++i) {
        for (size_t j = 1; j < pd.tickers_.size(); ++j) {
            dividendsSoFar += pd.dividends_[i][j] * result.originalShares[j];
        }
        double nav = dividendsSoFar;
        for (size_t j = 0; j < pd.tickers_.size(); ++j) {
            nav += pd.prices_[i][j] * result.originalShares[j];
        }
        ASSERTNE(nav, 0.0);
        result.dailyPrices.emplace_back(nav);
        double ret = (nav - originalNavSum) / originalNavSum;

        if (FLAGS_decay) {
            result.returnsStat.decay(DECAY_SPEED);
        }
        result.returnsStat.add(ret);
    }

    for (size_t i = 1; i < result.dailyPrices.size(); ++i) {
        if (FLAGS_decay) {
            result.dailyReturnsStat.decay(DECAY_SPEED);
            result.dailyNegReturnsStat.decay(DECAY_SPEED);
        }

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

    double concentration = 0.0;
    for (size_t j = 1; j < pd.tickers_.size(); ++j) {
        concentration += sqr(pd.prices_.back()[j] * result.originalShares[j] / result.dailyPrices.back());
    }

    result.sortino = result.dailyReturnsStat.mean() / result.dailyNegReturnsStat.stddev();
    result.concentration = concentration;
    result.additiveSortino = result.dailyReturnsStat.mean() - FLAGS_risk_weight * result.dailyNegReturnsStat.stddev() -
                             ((useConcentration) ? FLAGS_concentration_risk_weight * result.concentration : 0.0);

    result.finalNav.resize(pd.tickers_.size());
    for (size_t i = 0; i < pd.tickers_.size(); ++i) {
        result.finalNav[i] = result.originalShares[i] * pd.prices_.back()[i];
    }
    result.dividends = dividendsSoFar;
    result.finalNav[0] += dividendsSoFar;
    result.lastDate = pd.dates_.back();

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
         << ", sortino: " << res.sortino << ", return mean: " << res.dailyReturnsStat.mean()
         << ", neg return stddev: " << res.dailyNegReturnsStat.stddev() << ", dividends: " << res.dividends
         << ", f: " << res.f << ", last date: " << res.lastDate << endl;
}

void testModeling(const PriceData& pd) {
    cout << "Init prices: " << pd.prices_.front() << endl;
    cout << "Final prices: " << pd.prices_.back() << endl;
    Portfolio p(pd.tickers_.size(), 1);
    normalizeNavInplace(p);
    auto res = model(pd, p, true);
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

Portfolio loadPortfolio(const PriceData& pd, const string& filename) {
    auto df = DataFrame::loadFromCsv(filename);
    auto colSymbol = df->getColumn("Symbol");
    auto colValue = df->getColumn("Current Value");
    unordered_map<string, double> symbol2value;
    for (size_t i = 0; i < colSymbol->size(); ++i) {
        symbol2value.emplace(colSymbol->as(i), colValue->as<double>(i));
    }
    Portfolio result(pd.tickers_.size(), 0);
    for (size_t i = 0; i < pd.tickers_.size(); ++i) {
        const auto toSymbol = symbol2value.find(pd.tickers_[i]);
        if (toSymbol != symbol2value.end()) {
            result[i] = toSymbol->second / pd.prices_.back()[i] * pd.prices_.front()[i];
        }
    }
    normalizeNavInplace(result);
    return result;
}

ModelResult gradientSearch(const PriceData& pd) {
    Timer tSolve("Gradient search");

    ModelResult bestRes;
    bestRes.originalNav.resize(pd.tickers_.size(), 1);
    normalizeNavInplace(bestRes.originalNav);

    mutex mtxEval;
    auto eval = [&](Portfolio& c) {
        if (sum(c) != 0) {
            normalizeNavInplace(c);
            const auto res = model(pd, c, true);
            mtxEval.lock();
            if (res.f > bestRes.f) {
                bestRes = res;
            }
            mtxEval.unlock();
        }
    };

    tp::ThreadPool tp;
    mutex mtxPrint;
    tp.runN(
        [&](int k) {
            ModelResult bestRes1;
            bestRes1.originalNav = randomPortfolio(pd.tickers_);

            auto eval1 = [&](Portfolio& c) {
                normalizeNavInplace(c);
                const auto res = model(pd, c, true);
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
                {
                    mtxPrint.lock();
                    cerr << k << " " << i << " " << bestRes1.f << " " << bestRes1.originalNav << endl;
                    mtxPrint.unlock();
                }
            }

            eval(bestRes1.originalNav);
        },
        FLAGS_iterations);

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

void dumpPricesToCsv(const PriceData& pd, const ModelResult& model, const string& filename) {
    Timer tSolve("Gen CSV");

    DataFrame df(cat(StringVector{"Date", "Optimal"}, pd.tickers_));

    vector<DoubleVector> prices(pd.dates_.size(), DoubleVector(1 + pd.tickers_.size()));
    double dividends = 0;
    for (size_t i = 0; i < pd.dates_.size(); ++i) {
        for (size_t j = 0; j < pd.tickers_.size(); ++j) {
            ALWAYS_ASSERT(isfinite(model.originalShares[j]));
            prices[i][0] += model.originalShares[j] * pd.prices_[i][j];
            dividends += model.originalShares[j] * pd.dividends_[i][j];
        }
        prices[i][0] += dividends;
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
        StringVector tickers;
        if (FLAGS_stocks) {
            tickers = cat(etfs, stocks);
        } else {
            tickers = etfs;
        }

        auto data = loadData(tickers);
        // testModeling(data);
        auto best = gradientSearch(data);
        dumpPricesToCsv(data, best, "optimalAll.csv");

        SizeTVector nonZeroIndices;
        auto nonZeroBest = best;
        size_t index = 0;
        for (size_t i = 0; i < data.tickers_.size(); ++i) {
            if (best.originalShares[i]) {
                nonZeroBest.originalShares[index++] = best.originalShares[i];
                nonZeroIndices.emplace_back(i);
            }
        }
        dumpPricesToCsv(data.subPriceData(nonZeroIndices), nonZeroBest, "optimalNZ.csv");
    } else if (FLAGS_mode == "optimize1") {
        const StringVector tickers = cat(etfs, stocks);
        const auto data = loadData(tickers);
        const auto p0 = loadPortfolio(data, FLAGS_input);
        LOG(INFO) << OUT(p0);
        const auto resP0 = model(data, p0, true);
        out(resP0);
        dumpPricesToCsv(data, resP0, "p0.csv");

        vector<pair<double, size_t>> results;
        double bestF = -1.0;
        ModelResult bestRes;
        for (size_t i = 0; i < data.tickers_.size(); ++i) {
            if (!FLAGS_stocks && has(stocks, data.tickers_[i])) {
                continue;
            }
            auto p = p0;
            static const double DX = 0.0001;
            p[i] += DX;
            normalizeNavInplace(p);
            const auto res = model(data, p, true);
            if (res.f > bestF) {
                bestF = res.f;
                bestRes = res;
            }
            results.emplace_back(make_pair((res.f - resP0.f) / DX, i));
        }
        sort(results);
        for (const auto& r : results) {
            const auto& ticker = data.tickers_[r.second];

            double stockF = 0;
            {
                Portfolio stockP(data.tickers_.size());
                stockP[r.second] = 1.0;
                normalizeNavInplace(stockP);
                const auto resStock = model(data, stockP, false);
                stockF = resStock.f;
            }

            cout << r.first << "\t" << ticker << "\t" << p0[r.second] << "\t" << has(stocks, ticker) << "\t" << stockF
                 << endl;
        }
        out(bestRes);
    } else {
        THROW("Unknown mode '" << FLAGS_mode << "'");
    }

    return 0;
}
