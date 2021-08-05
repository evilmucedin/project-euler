#include <set>

#include "lib/header.h"
#include "lib/timer.h"
#include "lib/stat.h"
#include "lib/ml/dataframe.h"

static const StringVector tickers = {"FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "GOOG", "IWM", "VUG",
                                     "MSFT",  "T",     "NCLH",  "OGZPY", "SPY",   "IVV",   "VOO",  "QQQ"};

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
        }
    }

    result.dates_.insert(result.dates_.end(), dates.begin(), dates.end());

    return result;
}

using Portfolio = DoubleVector;

void normalizeNavInplace(Portfolio& p) {
    double sm = sum(p);
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
};

double sharpe(const ModelResult& res) {
    return (sum(res.finalNav) - sum(res.originalNav)) / sum(res.originalNav) / res.returnsStat.stddev();
}

ModelResult model(const PriceData& pd, const Portfolio& originalNav) {
    ModelResult result;
    result.originalNav = originalNav;
    result.originalShares.resize(tickers.size());
    for (size_t i = 0; i < tickers.size(); ++i) {
        result.originalShares[i] = result.originalNav[i] / pd.prices_.front()[i];
    }

    for (size_t i = 0; i < pd.prices_.size(); ++i) {
        for (size_t j = 0; j < tickers.size(); ++j) {
            double ret = pd.prices_[i][j]*result.originalShares[j] - result.originalNav[j];
            result.returnsStat.add(ret);
        }
    }

    result.finalNav.resize(tickers.size());
    for (size_t i = 0; i < tickers.size(); ++i) {
        result.finalNav[i] = result.originalShares[i] * pd.prices_.back()[i];
    }

    result.sharpe = sharpe(result);

    return result;
}

void out(const ModelResult& res) {
    const auto before = sum(res.originalNav);
    const auto after = sum(res.finalNav);
    cout << res.returnsStat << ", initial NAV: " << before << ", final NAV: " << after << ", sharpe: " << res.sharpe
         << endl;
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

void gradientSearch(const PriceData& pd) {
    ModelResult bestRes;
    bestRes.originalNav.resize(tickers.size(), 1);
    normalizeNavInplace(bestRes.originalNav);

    auto eval = [&](Portfolio& c) {
        normalizeNavInplace(c);
        const auto res = model(pd, c);
        if (res.sharpe > bestRes.sharpe) {
            bestRes = res;
        }
    };

    for (size_t k = 0; k < 10; ++k) {
        ModelResult bestRes1;
        bestRes1.originalNav = randomPortfolio();

        auto eval1 = [&](Portfolio& c) {
            normalizeNavInplace(c);
            const auto res = model(pd, c);
            if (res.sharpe > bestRes1.sharpe) {
                bestRes1 = res;
            }
        };

        for (size_t i = 0; i < 200; ++i) {
            for (size_t j = 0; j < tickers.size(); ++j) {
                Portfolio c = bestRes.originalNav;
                c[j] *= 0.995;
                eval(c);
                c = bestRes.originalNav;
                c[j] *= 1.005;
                eval(c);
            }
            cerr << i << " " << bestRes.sharpe << " " << bestRes.originalNav << endl;
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
}

int main() {
    auto data = loadData();
    // testModeling(data);
    gradientSearch(data);
    return 0;
}
