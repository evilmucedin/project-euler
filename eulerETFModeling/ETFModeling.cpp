#include <set>

#include "lib/header.h"
#include "lib/timer.h"
#include "lib/ml/dataframe.h"

static const StringVector tickers = {"FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "GOOG", "IWM", "VUG", "MSFT"};

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
    return result;
}

int main() {
    loadData();
    return 0;
}
