#include "ETFModeling/etfModel.h"

#include <cmath>
#include <cstdlib>
#include <fstream>

#include "lib/exception.h"
#include "lib/ml/dataframe.h"

#include "gtest/gtest.h"

namespace {

// Builds price data the way loadData does: column 0 is the cash position
// "CORE**" at a constant price of 1 and no dividends.
PriceData makePriceData(const StringVector& tickers, const vector<DoubleVector>& prices,
                        const vector<DoubleVector>& dividends = {}) {
    PriceData pd;
    pd.tickers_ = cat(StringVector{"CORE**"}, tickers);
    for (size_t i = 0; i < prices.size(); ++i) {
        pd.dates_.emplace_back("2020-01-" + string(i < 9 ? "0" : "") + to_string(i + 1));
        pd.prices_.emplace_back(cat(DoubleVector{1.0}, prices[i]));
        DoubleVector div(pd.tickers_.size());
        if (!dividends.empty()) {
            for (size_t j = 0; j < tickers.size(); ++j) {
                div[j + 1] = dividends[i][j];
            }
        }
        pd.dividends_.emplace_back(std::move(div));
    }
    return pd;
}

// A scratch directory laid out as loadData expects: <dir>/marketData/*.csv.
class TempDataFolder {
   public:
    TempDataFolder() {
        char pattern[] = "/tmp/etfModelTestXXXXXX";
        const char* dir = mkdtemp(pattern);
        if (!dir) {
            THROW("mkdtemp failed");
        }
        dir_ = dir;
        system(("mkdir -p " + dir_ + "/marketData").c_str());
    }

    ~TempDataFolder() { system(("rm -rf " + dir_).c_str()); }

    string folder() const { return dir_ + "/"; }

    string path(const string& name) const { return dir_ + "/" + name; }

    void write(const string& name, const string& content) const {
        std::ofstream f(path(name));
        f << content;
    }

    void writeTicker(const string& ticker, const string& content) const {
        write("marketData/" + ticker + ".csv", content);
    }

   private:
    string dir_;
};

class EtfModelTest : public ::testing::Test {
   protected:
    void SetUp() override {
        FLAGS_first_date = "";
        FLAGS_additive_sortino = true;
        FLAGS_risk_weight = 0.05;
        FLAGS_decay = false;
        FLAGS_concentration_risk_weight = 0.0;
        FLAGS_iterations = 2;
    }

    google::FlagSaver flagSaver_;
};

}  // namespace

TEST_F(EtfModelTest, NormalizeNavSumsToOne) {
    Portfolio p = {1.0, 3.0, 4.0};
    normalizeNavInplace(p);
    EXPECT_DOUBLE_EQ(p[0], 0.125);
    EXPECT_DOUBLE_EQ(p[1], 0.375);
    EXPECT_DOUBLE_EQ(p[2], 0.5);
}

TEST_F(EtfModelTest, RandomPortfolioIsNormalizedAndNonNegative) {
    const auto p = randomPortfolio({"A", "B", "C", "D"});
    ASSERT_EQ(p.size(), 4u);
    EXPECT_NEAR(sum(p), 1.0, 1e-12);
    for (auto x : p) {
        EXPECT_GE(x, 0.0);
    }
}

TEST_F(EtfModelTest, SubPriceDataSelectsColumns) {
    auto pd = makePriceData({"A", "B"}, {{10, 20}, {11, 22}}, {{0.1, 0.2}, {0.3, 0.4}});
    const auto sub = pd.subPriceData({0, 2});
    EXPECT_EQ(sub.tickers_, (StringVector{"CORE**", "B"}));
    EXPECT_EQ(sub.dates_, pd.dates_);
    ASSERT_EQ(sub.prices_.size(), 2u);
    EXPECT_EQ(sub.prices_[1], (DoubleVector{1.0, 22.0}));
    EXPECT_EQ(sub.dividends_[1], (DoubleVector{0.0, 0.4}));
}

TEST_F(EtfModelTest, LoadDataAlignsDatesAndFillsGaps) {
    TempDataFolder data;
    // A is missing 01-03, B is missing 01-02 and the first date 01-01.
    data.writeTicker("A",
                     "Date,Adj Close,Dividends\n"
                     "2020-01-01,10,0\n"
                     "2020-01-02,11,0.5\n"
                     "2020-01-04,13,0\n");
    data.writeTicker("B",
                     "Date,Adj Close,Dividends\n"
                     "2020-01-03,50,1\n"
                     "2020-01-04,52,0\n");
    FLAGS_dataFolder = data.folder();

    const auto pd = loadData({"A", "B"});

    EXPECT_EQ(pd.tickers_, (StringVector{"CORE**", "A", "B"}));
    EXPECT_EQ(pd.dates_, (StringVector{"2020-01-01", "2020-01-02", "2020-01-03", "2020-01-04"}));
    ASSERT_EQ(pd.prices_.size(), 4u);
    for (const auto& row : pd.prices_) {
        EXPECT_EQ(row[0], 1.0);
    }
    // A: a missing date carries the previous price forward.
    EXPECT_EQ(pd.prices_[2][1], 11.0);
    EXPECT_EQ(pd.prices_[3][1], 13.0);
    // B: dates before its history starts take the first known price.
    EXPECT_EQ(pd.prices_[0][2], 50.0);
    EXPECT_EQ(pd.prices_[1][2], 50.0);
    // Dividends are not carried: a missing date pays nothing.
    EXPECT_EQ(pd.dividends_[1][1], 0.5);
    EXPECT_EQ(pd.dividends_[2][1], 0.0);
    EXPECT_EQ(pd.dividends_[2][2], 1.0);
    EXPECT_EQ(pd.dividends_[0][2], 0.0);
}

TEST_F(EtfModelTest, LoadDataHonorsFirstDate) {
    TempDataFolder data;
    data.writeTicker("A",
                     "Date,Adj Close,Dividends\n"
                     "2019-12-31,9,0\n"
                     "2020-01-01,10,0\n"
                     "2020-01-02,11,0\n");
    FLAGS_dataFolder = data.folder();
    FLAGS_first_date = "2020-01-01";

    const auto pd = loadData({"A"});

    EXPECT_EQ(pd.dates_, (StringVector{"2020-01-01", "2020-01-02"}));
    EXPECT_EQ(pd.prices_.front()[1], 10.0);
}

TEST_F(EtfModelTest, LoadDataThrowsWithoutDividends) {
    TempDataFolder data;
    data.writeTicker("A",
                     "Date,Adj Close\n"
                     "2020-01-01,10\n");
    FLAGS_dataFolder = data.folder();

    EXPECT_THROW(loadData({"A"}), std::exception);
}

TEST_F(EtfModelTest, LoadDataReadsCommittedMarketData) {
    // The real SPY history shipped with the app, via the path used in production.
    FLAGS_dataFolder = "ETFModeling/";
    std::ifstream probe(FLAGS_dataFolder + "marketData/SPY.csv");
    if (!probe) {
        GTEST_SKIP() << "run from the repository root to use ETFModeling/marketData";
    }
    FLAGS_first_date = "2024-01-01";

    const auto pd = loadData({"SPY"});

    ASSERT_GT(pd.dates_.size(), 100u);
    EXPECT_GE(pd.dates_.front(), "2024-01-01");
    for (size_t i = 0; i < pd.prices_.size(); ++i) {
        EXPECT_GT(pd.prices_[i][1], 0.0);
        EXPECT_GE(pd.dividends_[i][1], 0.0);
    }
}

TEST_F(EtfModelTest, ModelAllCashIsFlat) {
    const auto pd = makePriceData({"A"}, {{10}, {20}, {5}});
    const auto res = model(pd, {1.0, 0.0}, true);

    EXPECT_EQ(res.dailyPrices, (DoubleVector{1.0, 1.0, 1.0}));
    EXPECT_EQ(res.finalNav, (DoubleVector{1.0, 0.0}));
    EXPECT_EQ(res.concentration, 0.0);
    EXPECT_EQ(res.dailyReturnsStat.mean(), 0.0);
    EXPECT_EQ(res.additiveSortino, 0.0);
    EXPECT_EQ(res.f, res.additiveSortino);
    EXPECT_EQ(res.lastDate, pd.dates_.back());
}

TEST_F(EtfModelTest, ModelTracksSharesThroughPriceChanges) {
    const auto pd = makePriceData({"A", "B"}, {{10, 4}, {20, 2}, {15, 8}});
    const auto res = model(pd, {0.0, 0.5, 0.5}, true);

    EXPECT_DOUBLE_EQ(res.originalShares[1], 0.05);
    EXPECT_DOUBLE_EQ(res.originalShares[2], 0.125);
    ASSERT_EQ(res.dailyPrices.size(), 3u);
    EXPECT_DOUBLE_EQ(res.dailyPrices[0], 1.0);
    EXPECT_DOUBLE_EQ(res.dailyPrices[1], 1.25);
    EXPECT_DOUBLE_EQ(res.dailyPrices[2], 1.75);
    EXPECT_DOUBLE_EQ(res.finalNav[1], 0.75);
    EXPECT_DOUBLE_EQ(res.finalNav[2], 1.0);
    ASSERT_EQ(res.dailyReturns.size(), 2u);
    EXPECT_DOUBLE_EQ(res.dailyReturns[0], log(1.25));
    EXPECT_DOUBLE_EQ(res.dailyReturns[1], log(1.75 / 1.25));
}

TEST_F(EtfModelTest, ModelCreditsDividendsToCash) {
    const auto pd = makePriceData({"A"}, {{10}, {10}, {10}}, {{0.0}, {1.0}, {2.0}});
    const auto res = model(pd, {0.0, 1.0}, true);

    // 0.1 shares receive 0.1 and then 0.2; dividends accumulate and are not reinvested.
    EXPECT_DOUBLE_EQ(res.dividends, 0.3);
    EXPECT_DOUBLE_EQ(res.finalNav[0], 0.3);
    EXPECT_DOUBLE_EQ(res.finalNav[1], 1.0);
    EXPECT_DOUBLE_EQ(res.dailyPrices[1], 1.1);
    EXPECT_DOUBLE_EQ(res.dailyPrices[2], 1.3);
}

TEST_F(EtfModelTest, ModelSteadyGrowthHasNoDownsideRisk) {
    vector<DoubleVector> prices;
    double price = 100;
    for (int i = 0; i < 10; ++i) {
        prices.push_back({price});
        price *= 1.01;
    }
    const auto pd = makePriceData({"A"}, prices);
    const auto res = model(pd, {0.0, 1.0}, true);

    EXPECT_NEAR(res.dailyReturnsStat.mean(), log(1.01), 1e-12);
    EXPECT_NEAR(res.dailyNegReturnsStat.stddev(), 0.0, 1e-9);
    EXPECT_NEAR(res.additiveSortino, log(1.01), 1e-9);
    EXPECT_EQ(res.f, res.additiveSortino);
}

TEST_F(EtfModelTest, ModelRiskWeightPenalizesDownside) {
    const auto pd = makePriceData({"A"}, {{10}, {8}, {12}, {9}, {13}});
    FLAGS_risk_weight = 0.0;
    const auto noRisk = model(pd, {0.0, 1.0}, true);
    FLAGS_risk_weight = 1.0;
    const auto withRisk = model(pd, {0.0, 1.0}, true);

    EXPECT_GT(noRisk.dailyNegReturnsStat.stddev(), 0.0);
    EXPECT_DOUBLE_EQ(noRisk.additiveSortino, noRisk.dailyReturnsStat.mean());
    EXPECT_DOUBLE_EQ(withRisk.additiveSortino,
                     withRisk.dailyReturnsStat.mean() - withRisk.dailyNegReturnsStat.stddev());
}

TEST_F(EtfModelTest, ModelConcentration) {
    const auto pd = makePriceData({"A", "B"}, {{10, 10}, {12, 12}});
    EXPECT_DOUBLE_EQ(model(pd, {0.0, 1.0, 0.0}, true).concentration, 1.0);
    EXPECT_DOUBLE_EQ(model(pd, {0.0, 0.5, 0.5}, true).concentration, 0.5);

    FLAGS_concentration_risk_weight = 0.1;
    const auto with = model(pd, {0.0, 1.0, 0.0}, true);
    const auto without = model(pd, {0.0, 1.0, 0.0}, false);
    EXPECT_NEAR(without.additiveSortino - with.additiveSortino, 0.1, 1e-12);
}

TEST_F(EtfModelTest, ModelSortinoMode) {
    const auto pd = makePriceData({"A"}, {{10}, {8}, {12}, {9}, {13}});
    FLAGS_additive_sortino = false;
    const auto res = model(pd, {0.0, 1.0}, true);

    EXPECT_DOUBLE_EQ(res.sortino, res.dailyReturnsStat.mean() / res.dailyNegReturnsStat.stddev());
    EXPECT_EQ(res.f, res.sortino);
}

TEST_F(EtfModelTest, ModelDecayWeighsRecentReturnsMore) {
    // A crash at the start followed by recovery: with decay the early loss counts less.
    const auto pd = makePriceData({"A"}, {{10}, {5}, {6}, {7}, {8}, {9}});
    FLAGS_decay = false;
    const auto flat = model(pd, {0.0, 1.0}, true);
    FLAGS_decay = true;
    const auto decayed = model(pd, {0.0, 1.0}, true);

    EXPECT_GT(decayed.dailyReturnsStat.mean(), flat.dailyReturnsStat.mean());
}

TEST_F(EtfModelTest, CalcReturnsComputesLogReturnsAndSharpe) {
    ModelResult res;
    res.dailyPrices = {100, 110, 99, 108.9};
    res.calcReturns();

    ASSERT_EQ(res.dailyReturns.size(), 3u);
    EXPECT_DOUBLE_EQ(res.dailyReturns[0], log(1.1));
    EXPECT_DOUBLE_EQ(res.dailyReturns[1], log(0.9));
    EXPECT_DOUBLE_EQ(res.dailyReturns[2], log(1.1));
    EXPECT_DOUBLE_EQ(res.dailySharpe, res.dailyReturnsStat.mean() / res.dailyReturnsStat.stddev());

    // Calling again must not accumulate.
    res.calcReturns();
    EXPECT_EQ(res.dailyReturns.size(), 3u);
}

TEST_F(EtfModelTest, LoadPortfolioConvertsValuesToStartingWeights) {
    TempDataFolder data;
    data.write("portfolio.csv",
               "Symbol,Current Value\n"
               "A,300\n"
               "B,100\n"
               "UNKNOWN,1000\n");
    // A doubled and B was flat, so today's 3:1 split was 1.5:1 at the start.
    const auto pd = makePriceData({"A", "B"}, {{10, 5}, {20, 5}});

    const auto p = loadPortfolio(pd, data.path("portfolio.csv"));

    ASSERT_EQ(p.size(), 3u);
    EXPECT_DOUBLE_EQ(p[0], 0.0);
    EXPECT_DOUBLE_EQ(p[1], 0.6);
    EXPECT_DOUBLE_EQ(p[2], 0.4);

    // Modeling the loaded portfolio reproduces the current split.
    const auto res = model(pd, p, true);
    EXPECT_NEAR(res.finalNav[1] / res.finalNav[2], 3.0, 1e-12);
}

TEST_F(EtfModelTest, DumpPricesToCsvWritesNormalizedSeries) {
    TempDataFolder data;
    const auto pd = makePriceData({"A", "B"}, {{10, 4}, {20, 2}, {15, 8}}, {{0, 0}, {1, 0}, {0, 0}});
    const auto res = model(pd, {0.0, 0.5, 0.5}, true);

    dumpPricesToCsv(pd, res, data.path("out.csv"));
    // DataFrame::saveToCsv leaves its FILE* open (File has no destructor), so
    // the output only reaches the disk on exit unless flushed here.
    fflush(nullptr);

    const auto df = DataFrame::loadFromCsv(data.path("out.csv"));
    EXPECT_EQ(df->columnNames(), (StringVector{"Date", "Optimal", "CORE**", "A", "B"}));
    ASSERT_EQ(df->numLines(), 3u);
    const auto dates = df->getColumn("Date", "", "");
    const auto optimal = df->getColumn("Optimal", "", "");
    const auto a = df->getColumn("A", "", "");
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(dates->as<string>(i), pd.dates_[i]);
        // The portfolio line is the modeled NAV, dividends included.
        EXPECT_NEAR(optimal->as<double>(i), res.dailyPrices[i] / res.dailyPrices[0], 1e-6);
        EXPECT_NEAR(a->as<double>(i), pd.prices_[i][1] / pd.prices_[0][1], 1e-6);
    }
}

TEST_F(EtfModelTest, GradientSearchFavorsSteadyWinner) {
    vector<DoubleVector> prices;
    double up = 100;
    double down = 100;
    for (int i = 0; i < 30; ++i) {
        // B zigzags downwards, so it loses on both return and downside risk.
        prices.push_back({up, down * ((i & 1) ? 1.05 : 0.95)});
        up *= 1.005;
        down *= 0.99;
    }
    const auto pd = makePriceData({"UP", "DOWN"}, prices);

    const auto best = gradientSearch(pd);

    EXPECT_NEAR(sum(best.originalNav), 1.0, 1e-12);
    EXPECT_GT(best.originalNav[1], 0.9);
    EXPECT_DOUBLE_EQ(best.f, model(pd, best.originalNav, true).f);

    Portfolio equal(pd.tickers_.size(), 1.0);
    normalizeNavInplace(equal);
    EXPECT_GT(best.f, model(pd, equal, true).f);
}

TEST_F(EtfModelTest, MinMaxStrategyTradesTurningPoints) {
    const auto pd = makePriceData({"A"}, {{1}, {2}, {1}, {2}, {1}});

    const auto res = minMaxStrategy(pd, 1);

    EXPECT_EQ(res.buyAndHold.dailyPrices, (DoubleVector{1, 2, 1, 2, 1}));
    // Starts in cash, buys the dip on day 2 and sells the peak on day 3.
    EXPECT_EQ(res.minMax.dailyPrices, (DoubleVector{1, 1, 1, 2, 2}));
    EXPECT_EQ(res.minMax.dailyReturns.size(), 4u);
    EXPECT_GT(res.minMax.dailyReturnsStat.mean(), res.buyAndHold.dailyReturnsStat.mean());
}
