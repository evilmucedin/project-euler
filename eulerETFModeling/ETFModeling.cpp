#include <set>

#include "lib/header.h"
#include "lib/timer.h"
#include "lib/stat.h"
#include "lib/ml/dataframe.h"
#include "lib/random.h"

/*
static const StringVector tickers = {"FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "GOOG",
                                     "IWM",   "VUG",   "MSFT",  "T",     "NCLH",  "OGZPY", "SPY",
                                     "IVV",   "VOO",   "QQQ",   "AMZN",  "FB",    "TSLA"};
*/

static const StringVector tickers = {"FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "IWM", "VUG", "SPY",
                                     "IVV",   "VOO",   "QQQ",   "BND",   "FBND",  "HDV",   "VEU", "VWO"};
/*
*/

// static const StringVector tickers = {"TSLA"};

struct PriceData {
    vector<DoubleVector> prices_;
    vector<string> dates_;
};

PriceData loadData() {
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
                tickerPrices[iDate] = price->as<double>(i);
                dates.emplace(iDate);
            }
            mappedData.emplace_back(std::move(tickerPrices));
        }
    }

    PriceData result;
    result.prices_.resize(dates.size());
    for (auto& col : result.prices_) {
        col.resize(tickers.size());
    }
    size_t iDate = 0;
    for (const auto& date: dates) {
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
    for (auto& x: p) {
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
    double f{};
};

double sharpe(const ModelResult& res) {
    return (sum(res.finalNav) - sum(res.originalNav)) / sum(res.originalNav) / res.returnsStat.stddev();
    // return (sum(res.finalNav) - sum(res.originalNav)) / sum(res.originalNav) - 5 * res.returnsStat.stddev();
}

ModelResult model(const PriceData& pd, const Portfolio& originalNav) {
    ModelResult result;
    result.originalNav = originalNav;
    result.originalShares.resize(tickers.size());
    for (size_t i = 0; i < tickers.size(); ++i) {
        result.originalShares[i] = result.originalNav[i] / pd.prices_.front()[i];
        ALWAYS_ASSERT(isfinite(result.originalShares[i]));
    }
    const double originalNavSum = sum(originalNav);

    result.dailyPrices.reserve(pd.prices_.size());
    result.dailyReturns.reserve(pd.prices_.size());
    for (size_t i = 0; i < pd.prices_.size(); ++i) {
        double nav = 0.0;
        for (size_t j = 0; j < tickers.size(); ++j) {
            nav += pd.prices_[i][j] * result.originalShares[j];
        }
        result.dailyPrices.emplace_back(nav);
        double ret = (nav - originalNavSum) / originalNavSum;
        result.returnsStat.add(ret);
    }

    for (size_t i = 1; i < result.dailyPrices.size(); ++i) {
        const double dailyRet = log(result.dailyPrices[i]/result.dailyPrices[i - 1]);
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

    result.finalNav.resize(tickers.size());
    for (size_t i = 0; i < tickers.size(); ++i) {
        result.finalNav[i] = result.originalShares[i] * pd.prices_.back()[i];
    }

    result.sharpe = sharpe(result);
    result.dailySharpe = result.dailyReturnsStat.mean() / result.dailyReturnsStat.stddev();

    result.f = result.sharpe;

    return result;
}

void out(const ModelResult& res) {
    const auto before = sum(res.originalNav);
    const auto after = sum(res.finalNav);
    cout << res.returnsStat << ", initial NAV: " << before << ", final NAV: " << after << ", sharpe: " << res.sharpe
         << ", f: " << res.f << endl;
}

void testModeling(const PriceData& pd) {
    cout << "Init prices: " << pd.prices_.front() << endl;
    cout << "Final prices: " << pd.prices_.back() << endl;
    Portfolio p(tickers.size(), 1);
    normalizeNavInplace(p);
    auto res = model(pd, p);
    out(res);
}

Portfolio randomPortfolio() {
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
    bestRes.originalNav.resize(tickers.size(), 1);
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

    for (size_t k = 0; k < 10; ++k) {
        ModelResult bestRes1;
        bestRes1.originalNav = randomPortfolio();

        auto eval1 = [&](Portfolio& c) {
            normalizeNavInplace(c);
            const auto res = model(pd, c);
            if (res.f > bestRes1.f) {
                bestRes1 = res;
            }
        };

        for (size_t i = 0; i < 200; ++i) {
            for (size_t j = 0; j < tickers.size(); ++j) {
                Portfolio c = bestRes1.originalNav;
                c[j] *= 0.995;
                eval1(c);
                c = bestRes1.originalNav;
                c[j] *= 1.005;
                eval1(c);
            }
            cerr << k << " " << i << " " << bestRes1.f << " " << bestRes1.originalNav << endl;
        }

        eval(bestRes1.originalNav);
    }

    for (size_t j = 0; j < tickers.size(); ++j) {
        auto c = bestRes.originalNav;
        c[j] = 0;
        eval(c);
    }

    for (size_t i = 0; i < tickers.size(); ++i) {
        cout << tickers[i] << " " << bestRes.originalNav[i] << " " << bestRes.finalNav[i] << endl;
    }
    out(bestRes);

    return bestRes;
}

void dumpPricesToCsv(const PriceData& pd, const ModelResult& model) {
    Timer tSolve("Gen CSV");

    DataFrame df(cat(StringVector{"Date", "Optimal"}, tickers));

    vector<DoubleVector> prices(pd.dates_.size(), DoubleVector(1 + tickers.size()));
    for (size_t i = 0; i < pd.dates_.size(); ++i) {
        for (size_t j = 0; j < tickers.size(); ++j) {
            ALWAYS_ASSERT(isfinite(model.originalShares[j]));
            prices[i][0] += model.originalShares[j] * pd.prices_[i][j];
        }
    }
    for (size_t i = 0; i < pd.dates_.size(); ++i) {
        for (size_t j = 0; j < tickers.size(); ++j) {
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

    df.saveToCsv("optimal.csv");
}

int main() {
    auto data = loadData();
    // testModeling(data);
    auto best = gradientSearch(data);
    dumpPricesToCsv(data, best);

    return 0;
}
