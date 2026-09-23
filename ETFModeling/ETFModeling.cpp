#include "gflags/gflags.h"
#include "glog/logging.h"

#include "ETFModeling/etfModel.h"
#include "lib/exception.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_string(mode, "optimize", "mode (optimize, optimize1)");
DEFINE_bool(portfolio_print, false, "portfolio print");
DEFINE_string(input, "portfolio input", "");
DEFINE_string(tickers, "tickers", "");
DEFINE_bool(stocks, false, "add stocks");


static const StringVector etfs = {
    "FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "IWM",  "VUG",  "SPY",  "IVV",   "VOO",  "QQQ",
    "BND",   "FBND",  "HDV",   "VEU",   "VWO",   "FDHY",  "FDIS", "ONEQ", "VV",   "VB",    "HNDL", "WBII",
    "PCEF",  "FDIV",  "CEFS",  "YLD",   "INKM",  "IYLD",  "FCEF", "MLTI", "YYY",  "MDIV",  "HIPS", "CVY",
    "GYLD",  "VTI",   "VEA",   "IEFA",  "AGG",   "GLD",   "XLF",  "VNQ",  "LQD",  "SWPPX", "MGK",  "UNG",
    "OIH",   "XME",   "PFIX",  "VXX",   "EWZ",   "ILF",   "SCHE", "FBCG", "FQAL", "FLPSX", "FDRR", "FMAG",
    "FPRO",  "FBCV",  "FMIL",  "BITO",  "BITW",  "VBB",   "SFY",  "IJR",  "SCHD", "FTLS",  "FDHT", "FRNW",
    "FDRV",  "FCLD",  "FDIG",  "ARKK",  "ITA",   "PPA",   "XAR",  "USO",  "IEO",  "SPGP",  "IWY",  "SPYG",
    "FELG",  "FTEC",  "FDVV", "DBO", "AIQ", "VDE",

};

static const StringVector stocks = {
    "GOOG", "GOOGL", "MSFT", "T",  "NCLH", "AMZN", "META", "AVGO", "AMD", "TSLA", "GME",  "AAPL",  "NVDA", "TSM",  "UNH", "JNJ", "V",
    "WMT",  "JPM",  "PG", "XOM",  "HD",   "CVX",  "PFE",  "COIN", "VWAGY", "LMT", "KHC", "NKE", "SBUX", "SHOP",
    "NBIS", "PLTR", "FLOC", "COP", "AIG",
};

// static const StringVector tickers = etfs;

/*
 */

// static const StringVector tickers = {"TSLA"};

void testModeling(const PriceData& pd) {
    cout << "Init prices: " << pd.prices_.front() << endl;
    cout << "Final prices: " << pd.prices_.back() << endl;
    Portfolio p(pd.tickers_.size(), 1);
    normalizeNavInplace(p);
    auto res = model(pd, p, true);
    out(res);
}

void optimize() {
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
}

void optimize1() {
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

        if (!FLAGS_portfolio_print || p0[r.second] > 0)
            cout << r.first << "\t" << ticker << "\t" << p0[r.second] << "\t" << has(stocks, ticker) << "\t" << stockF
                << endl;
    }
    out(bestRes);
}

void testStrategy() {
    const StringVector tickers = split(FLAGS_tickers, ',');
    const auto pd = loadData(tickers);
    for (size_t i = 0; i < pd.tickers_.size(); ++i) {
        const auto res = minMaxStrategy(pd, i);
        cout << res.buyAndHold.dailySharpe << "\t" << res.minMax.dailySharpe << "\t" << pd.tickers_[i] << endl;
    }
}


int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    {
        string sArgs;
        for (int i = 0; i < argc; ++i) {
            if (sArgs.size() > 0)
                sArgs += " ";
            sArgs += argv[i];
        }
        cout << "Args: " << sArgs << endl;
    }

    if (FLAGS_mode == "optimize") {
        optimize();
    } else if (FLAGS_mode == "optimize1") {
        optimize1();
    } else if (FLAGS_mode == "testStrategy") {
        testStrategy();
    } else {
        THROW("Unknown mode '" << FLAGS_mode << "'");
    }

    return 0;
}
