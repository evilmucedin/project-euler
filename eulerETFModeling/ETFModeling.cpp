#include "lib/header.h"

#include "lib/ml/dataframe.h"

static const StringVector tickers = {"FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "GOOG", "IWM", "VUG", "MSFT"};

void loadData() {
    for (const auto& ticker: tickers) {
        auto df = DataFrame::loadFromCsv(ticker + ".csv");
        cout << *df << endl;
    }
}

int main() {
    loadData();
    return 0;
}
