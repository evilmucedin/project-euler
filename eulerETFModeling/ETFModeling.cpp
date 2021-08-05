#include <set>

#include "lib/header.h"
#include "lib/timer.h"
#include "lib/stat.h"
#include "lib/ml/dataframe.h"

static const StringVector tickers = {"FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "GOOG", "IWM", "VUG", "MSFT", "T"};

using PriceData = vector<DoubleVector>;

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

    PriceData result(dates.size());
    for (auto& col : result) {
        col.resize(tickers.size());
    }
    size_t iDate = 0;
    for (const auto& date: dates) {
        for (size_t i = 0; i < tickers.size(); ++i) {
            const auto toDate = mappedData[i].find(date);
            if (toDate != mappedData[i].end()) {
                result[iDate][i] = toDate->second;
            } else {
                if (iDate) {
                    result[iDate][i] = result[iDate - 1][i];
                }
            }
        }
        ++iDate;
    }

    for (ssize_t i = dates.size() - 2; i >= 0; --i) {
        for (size_t j = 0; j < tickers.size(); ++j) {
            if (!result[i][j]) {
                result[i][j] = result[i + 1][j];
            }
        }
    }

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
};

ModelResult model(const PriceData& pd, const Portfolio& originalNav) {
    ModelResult result;
    result.originalNav = originalNav;
    result.originalShares.resize(tickers.size());
    for (size_t i = 0; i < tickers.size(); ++i) {
        result.originalShares[i] = result.originalNav[i] / pd.front()[i];
    }
    for (size_t i = 0; i < pd.size(); ++i) {
        for (size_t j = 0; j < tickers.size(); ++j) {
            double ret = pd[i][j]*result.originalShares[j] - result.originalNav[j];
            result.returnsStat.add(ret);
        }
    }
    result.finalNav.resize(tickers.size());
    for (size_t i = 0; i < tickers.size(); ++i) {
        result.finalNav[i] = result.originalShares[i] * pd.back()[i];
    }
    return result;
}

double sharpe(const ModelResult& res) {
    return (sum(res.finalNav) - sum(res.originalNav)) / sum(res.originalNav) / res.returnsStat.stddev();
}

void out(const ModelResult& res) {
    const auto before = sum(res.originalNav);
    const auto after = sum(res.finalNav);
    cout << res.returnsStat << ", initial NAV: " << before << ", final NAV: " << after << ", sharpe: " << sharpe(res)
         << endl;
}

void testModeling(const PriceData& pd) {
    cout << "Init prices: " << pd.front() << endl;
    cout << "Final prices: " << pd.back() << endl;
    Portfolio p(tickers.size(), 1);
    normalizeNavInplace(p);
    auto res = model(pd, p);
    out(res);
}

void gradientSearch(const PriceData& pd) {
    Portfolio bestP(tickers.size(), 1);
    ModelResult bestRes;
    normalizeNavInplace(bestP);
    double bestSharpe = 0;

    auto eval = [&](Portfolio& c) {
        normalizeNavInplace(c);
        const auto res = model(pd, c);
        const auto s = sharpe(res);
        if (s > bestSharpe) {
            bestSharpe = s;
            bestP = c;
            bestRes = res;
        }
    };

    for (size_t i = 0; i < 1000; ++i) {
        for (size_t j = 0; j < tickers.size(); ++j) {
            Portfolio c = bestP;
            c[j] *= 0.995;
            eval(c);
            c = bestP;
            c[j] *= 1.005;
            eval(c);
        }
        cerr << i << " " << bestSharpe << " " << bestP << endl;
    }

    for (size_t i = 0; i < tickers.size(); ++i) {
        cout << tickers[i] << " " << bestP[i] << " " << bestRes.finalNav[i] << endl;
    }
    out(bestRes);
}

int main() {
    auto data = loadData();
    // testModeling(data);
    gradientSearch(data);
    return 0;
}
