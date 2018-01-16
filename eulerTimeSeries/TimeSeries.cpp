#include "glog/logging.h"
#include "gflags/gflags.h"
#include "eigen/Dense"
#include "armadillo/armadillo"

#include "lib/datetime.h"
#include "lib/fft.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/fstream.h"
#include "lib/io/utils.h"
#include "lib/io/zstream.h"
#include "lib/math.h"
#include "lib/matrix.h"
#include "lib/random.h"
#include "lib/string.h"
#include "lib/timer.h"

#include "DNN.h"

DEFINE_bool(generate, false, "parse raw Reuters data");
DEFINE_bool(fft, false, "fft");
DEFINE_bool(linear, false, "linear");
DEFINE_bool(dnn, false, "dnn");
DEFINE_int64(limit, numeric_limits<int64_t>::max(), "limit number of messages");
DEFINE_double(learning_rate, 1.0, "learning rate");
DEFINE_double(regularization, 0, "learning rate");
DEFINE_int64(epochs, 100, "epochs");
DEFINE_bool(parse_dnn, false, "parse features for DNN");

using namespace Eigen;

// static const string kStock = "AAPL.O";
// static const string kStock = "YNDX.O";
static const string kStock = "AAAP.O";
static const string kAaplFilename = kStock + ".tsv";
static const string kAaplTimeSeries = kStock + ".ts";
static const string kHistogram = "histogram.tsv";
static const string kFilename = homeDir() + "/Downloads/NSQ-2017-11-28-MARKETPRICE-Data-1-of-1.csv.gz";

struct Histogramer {
    void add(const string& ric, double volume) { storage_[ric].emplace_back(volume); }

    void dump() {
        OFStream fOut(kHistogram);
        for (auto& kv : storage_) {
            auto& values = kv.second;
            sort(values.begin(), values.end());

            auto p = [&](double percentile) {
                return string("\t") + to_string(values[static_cast<size_t>(percentile * values.size())]);
            };

            fOut << kv.first;
            for (auto percentile : {0.0, 0.683772233983162, 0.9, 0.9683772233983162, 0.99, 0.9968377223398316, 0.999,
                                    0.9996837722339832, 0.9999, 0.9999683772233983, 0.99999, 0.9999968377223398}) {
                fOut << p(percentile);
            }
            fOut << endl;
        }
    }

    unordered_map<string, DoubleVector> storage_;
};

struct BidAsk {
    double bid_{0};
    double ask_{0};
    double bidSize_{0};
    double askSize_{0};
    double quoteTime_{0};
    bool open_{false};

    void clear() {
        bid_ = 0;
        ask_ = 0;
        bidSize_ = 0;
        askSize_ = 0;
        quoteTime_ = 0;
    }
};

struct IReutersParserCallback {
    virtual void onTrade(const std::string& ric, const DateTime& dt, double price, double volume, const BidAsk& bidask,
                         int index) {}

    virtual void onQuote(const std::string& ric, const DateTime& dt, const BidAsk& bidask) {}

    virtual ~IReutersParserCallback() {}
};

struct ComplexReutersParserCallback : public IReutersParserCallback {
    void addCallback(unique_ptr<IReutersParserCallback> callback) { callbacks_.emplace_back(std::move(callback)); }

    void onTrade(const std::string& ric, const DateTime& dt, double price, double volume, const BidAsk& bidask,
                 int index) override {
        for (auto& c : callbacks_) {
            c->onTrade(ric, dt, price, volume, bidask, index);
        }
    }

    void onQuote(const std::string& ric, const DateTime& dt, const BidAsk& bidask) override {
        for (auto& c : callbacks_) {
            c->onQuote(ric, dt, bidask);
        }
    }

    vector<unique_ptr<IReutersParserCallback>> callbacks_;
};

struct HistogramerCallback : public IReutersParserCallback {
    Histogramer h_;

    void onTrade(const std::string& ric, const DateTime& dt, double price, double volume, const BidAsk& bidask,
                 int index) override {
        if (0 != volume) {
            h_.add(ric, volume);
        }
    }

    ~HistogramerCallback() { h_.dump(); }
};

struct DumpCallback : public IReutersParserCallback {
    DumpCallback() : fPriceLevelsOut("priceLevels.tsv") {}

    void onTrade(const std::string& ric, const DateTime& dt, double price, double volume, const BidAsk& bidask,
                 int index) override {
        double spread = bidask.bid_ - bidask.ask_;
        double bam = (bidask.bid_ + bidask.ask_) / 2;
        double priceLevel = 0;
        if (spread) {
            priceLevel = (price - bam) / spread;
        }
        fPriceLevelsOut << ric << "\t" << dt.str() << "\t" << dt.time_.time_ << "\t" << bidask.quoteTime_ << "\t"
                        << bidask.bid_ << "\t" << bidask.ask_ << "\t" << price << "\t" << priceLevel << "\t" << index
                        << std::endl;
    }

    OFStream fPriceLevelsOut;
};

struct DumpTSCallback : public IReutersParserCallback {
    DumpTSCallback() : fOut(kAaplFilename) {}

    void onTrade(const std::string& ric, const DateTime& dt, double price, double volume, const BidAsk& bidask,
                 int index) override {
        if (ric == kStock) {
            if (volume > 0) {
                fOut << dt.time_.time_ << "\t" << price << "\t" << volume << endl;
            }
        }
    }

    OFStream fOut;
};

void parseReuters(const std::string& filename, IReutersParserCallback& callback) {
    Timer tTotal("Parse Reuters");
    auto fIn = make_shared<IFStream>(filename, std::ifstream::binary);
    auto zIn = make_shared<ZIStream>(fIn);
    CsvParser reader(zIn);
    reader.readHeader();
    const int iFIDNumber = reader.getIndex("Number of FIDs");
    const int iType = reader.getIndex("UpdateType/Action");
    const int iDateTime = reader.getIndex("Date-Time");
    const int iFidName = reader.getIndex("FID Name");
    const int iFidValue = reader.getIndex("FID Value");
    const int iRic = reader.getIndex("#RIC");
    const int iIndex = reader.getIndex("Key/Msg Sequence Number");
    unordered_map<std::string, BidAsk> bidasks;

    OFStream fSubset("subset.csv");
    int64_t iMessage = 0;
    while (reader.readLine()) {
        if (iMessage >= FLAGS_limit) {
            break;
        }
        ++iMessage;

        int nFids = reader.getInt(iFIDNumber);
        auto recordType = reader.get(iType);
        auto index = reader.getInt(iIndex);
        if (recordType == "TRADE") {
            auto ric = reader.get(iRic);
            if (ric == kStock) {
                fSubset << reader.line() << std::endl;
            }
            auto timestamp = reader.get(iDateTime);
            DateTime dateTime(timestamp);
            double price = 0;
            double volume = 0;
            for (int iFid = 0; iFid < nFids; ++iFid) {
                reader.readLine();
                if (ric == kStock) {
                    fSubset << reader.line() << std::endl;
                }
                auto fidName = reader.get(iFidName);
                if (fidName == "TRDPRC_1") {
                    price = reader.getDouble(iFidValue);
                } else if (fidName == "TRDVOL_1") {
                    volume = reader.getDouble(iFidValue);
                }
            }
            callback.onTrade(ric, dateTime, price, volume, bidasks[ric], index);
            LOG_EVERY_MS(INFO, 5000) << OUT(ric) << OUT(dateTime.str()) << OUT(price) << OUT(volume) << OUT(iMessage);
        } else if (recordType == "QUOTE") {
            auto ric = reader.get(iRic);
            if (ric == kStock) {
                fSubset << reader.line() << std::endl;
            }
            auto timestamp = reader.get(iDateTime);
            DateTime dateTime(timestamp);
            auto& bidask = bidasks[ric];
            for (int iFid = 0; iFid < nFids; ++iFid) {
                reader.readLine();
                if (ric == kStock) {
                    fSubset << reader.line() << std::endl;
                }
                auto fidName = reader.get(iFidName);
                if (fidName == "BID") {
                    if (!reader.empty(iFidValue)) {
                        bidask.bid_ = reader.getDouble(iFidValue);
                        bidask.quoteTime_ = dateTime.time_.time_;
                    } else {
                        bidask.bid_ = 0;
                    }
                } else if (fidName == "ASK") {
                    if (!reader.empty(iFidValue)) {
                        bidask.ask_ = reader.getDouble(iFidValue);
                        bidask.quoteTime_ = dateTime.time_.time_;
                    } else {
                        bidask.ask_ = 0;
                    }
                } else if (fidName == "BIDSIZE") {
                    if (!reader.empty(iFidValue)) {
                        bidask.bidSize_ = reader.getDouble(iFidValue);
                    } else {
                        bidask.bidSize_ = 0;
                    }
                } else if (fidName == "ASKSIZE") {
                    if (!reader.empty(iFidValue)) {
                        bidask.askSize_ = reader.getDouble(iFidValue);
                    } else {
                        bidask.askSize_ = 0;
                    }
                }
            }
            callback.onQuote(ric, dateTime, bidask);
        } else if (recordType == "UNSPECIFIED") {
            auto ric = reader.get(iRic);
            auto timestamp = reader.get(iDateTime);
            DateTime dateTime(timestamp);
            auto& bidask = bidasks[ric];
            for (int iFid = 0; iFid < nFids; ++iFid) {
                reader.readLine();
                auto fidName = reader.get(iFidName);
                if (fidName == "INST_PHASE") {
                    if (!reader.empty(iFidValue)) {
                        int value = reader.getInt(iFidValue);
                        if (value == 14) {
                            bidask.clear();
                        }
                        bidask.open_ = value == 3;
                    }
                }
            }
            callback.onQuote(ric, dateTime, bidask);
        } else {
            reader.skipLines(nFids);
        }
    }
    LOG(INFO) << OUT(reader.line());
}

void generate() {
    ComplexReutersParserCallback callback;
    callback.addCallback(std::make_unique<DumpTSCallback>());
    callback.addCallback(std::make_unique<DumpCallback>());
    callback.addCallback(std::make_unique<HistogramerCallback>());
    parseReuters(kFilename, callback);
}

constexpr size_t kN = 60*16;

void produceTimeSeries() {
    Timer tTotal("Produce TimeSeries");
    IFStream fIn(kAaplFilename);
    string line;
    constexpr double kTimeMin = 0.60417;
    constexpr double kTimeMax = 0.875004;
    DoubleVector dv(kN);
    DoubleVector vol(kN);
    IntVector n(kN);
    while (fIn.readLine(line)) {
        auto tokens = split(line, '\t');
        double time = stod(tokens[0]);
        double price = stod(tokens[1]);
        double volume = stod(tokens[2]);
        size_t iBucket = (time - kTimeMin) * kN / (kTimeMax - kTimeMin);
        dv[iBucket] += price*volume;
        vol[iBucket] += volume;
        ++n[iBucket];
    }
    OFStream fOut(kAaplTimeSeries);
    double price = 0;
    for (size_t i = 0; i < kN; ++i) {
        if (vol[i] != 0) {
            price = dv[i]/vol[i];
        }
        fOut << i << "\t" << price << "\t" << vol[i] << "\t" << n[i] << endl;
    }
}

class SpectralPredictor {
   public:
    SpectralPredictor(size_t nPoints, size_t nDim, size_t nEigen)
        : nPoints_(nPoints),
          nDim_(nDim),
          nEigen_(nEigen),
          x_(nDim_, nPoints_),
          vStar_(nDim_ - 1, nEigen_),
          vTau_(1, nEigen_),
          q_(nDim_ - 1, 1) {}

    void addPoint(double value) {
        points_.emplace_back(value);
        while (points_.size() > nPoints_ - 1 + nDim_) {
            points_.pop_front();
        }
    }

    double predict(size_t steps) {
        if (points_.empty()) {
            return 0;
        }

        if (points_.size() != nPoints_ - 1 + nDim_) {
            return points_.back();
        }

        for (size_t i = 0; i < nDim_; ++i) {
            for (size_t j = 0; j < nPoints_; ++j) {
                x_(i, j) = points_[i + j];
            }
        }

        auto c = x_ * x_.transpose() / nPoints_;

        SelfAdjointEigenSolver<MatrixXd> esC(c);
        auto v = esC.eigenvectors();
        // LOG(INFO) << OUTLN(esC.eigenvalues());
        // LOG(INFO) << OUTLN(v);

        for (size_t i = 0; i < nDim_ - 1; ++i) {
            for (size_t j = 0; j < nEigen_; ++j) {
                vStar_(i, j) = v(i, nDim_ - 1 - j);
            }
        }

        for (size_t i = 0; i < nEigen_; ++i) {
            vTau_(0, i) = v(nDim_ - 1, nDim_ - 1 - i);
        }

        auto predictionM = (vTau_ * vStar_.transpose()) / (1.0 - (vTau_ * vTau_.transpose())(0, 0));
        // cout << OUTLN(predictionM) << OUTLN(predictionM.sum());

        DoubleVector computed;
        auto getPoint = [&](size_t index) {
            if (index < points_.size()) {
                return points_[index];
            }
            return computed[index - points_.size()];
        };

        for (size_t iStep = 0; iStep < steps; ++iStep) {
            for (size_t i = 0; i < nDim_ - 1; ++i) {
                q_(i, 0) = getPoint((nPoints_ - 1 + nDim_) + i - (nDim_ - 1) + iStep);
            }

            computed.emplace_back((predictionM * q_)(0, 0));
        }

        return computed.back();
    }

   private:
    size_t nPoints_;
    size_t nDim_;
    size_t nEigen_;
    MatrixXd x_;
    MatrixXd vStar_;
    MatrixXd vTau_;
    MatrixXd q_;
    deque<double> points_;
};

struct LinearPredictor {
   public:
    LinearPredictor(size_t nPoints, size_t nDim) : nPoints_(nPoints), nDim_(nDim), q_(nDim_ + 1) { q_[nDim_] = 1.0; }

    void addPoint(double value) {
        points_.emplace_back(value);
        while (points_.size() > nPoints_ - 1 + nDim_ + 1000) {
            points_.pop_front();
        }
    }

    double predict(size_t steps) {
        if (points_.empty()) {
            return 0;
        }

        if (points_.size() < nDim_ + steps + 40) {
            return points_.back();
        }

        VectorPoints points;
        for (size_t i = 0; i + nDim_ + steps < points_.size(); ++i) {
            VectorPoint point(nDim_ + 1);
            point.b_ = points_[i + nDim_ - 1 + steps];
            for (size_t j = 0; j < nDim_; ++j) {
                point.a_[j] = points_[i + j];
            }
            point.a_[nDim_] = 1.0;
            points.emplace_back(std::move(point));
        }

        auto b = linearRegression(points, points_[0] * points_[0] * points_.size() / 10000000);
        // LOG_EVERY_MS(INFO, 100) << OUTLN(b);
        // cout << OUTLN(b);

        DoubleVector computed;
        auto getPoint = [&](size_t index) {
            if (index < points_.size()) {
                return points_[index];
            }
            return computed[index - points_.size()];
        };

        for (size_t iStep = 0; iStep < steps; ++iStep) {
            for (size_t i = 0; i < nDim_; ++i) {
                q_[i] = getPoint((points_.size() - 1) + i - nDim_ + iStep);
            }

            computed.emplace_back(dot(b, q_));
        }
        // cout << OUTLN(computed);

        return computed.back();
    }

   private:
    size_t nPoints_;
    size_t nDim_;
    deque<double> points_;
    DoubleVector q_;
};

struct SGDPredictor {
   public:
    SGDPredictor(size_t nDim, size_t nSteps) : nDim_(nDim), nSteps_(nSteps), nIt_(0), q_(nDim_ + 1), x_(nDim_ + 1), sg_(nDim_ + 1) {
        for (auto& x : q_) {
            x = randAB<double>(-1.0, 1.0) / 100;
        }
        x_[0] = 1;
    }

    void addPoint(double value) {
        points_.emplace_back(value);
        while (points_.size() > nSteps_ + nDim_) {
            points_.pop_front();
        }

        if (points_.size() == nSteps_ + nDim_) {
            ++nIt_;

            for (size_t i = 0; i < nDim_; ++i) {
                x_[i + 1] = points_[i];
            }

            double y = mul();
            double err = y - points_.back();
            // cout << OUT(x_) << OUTLN(q_);

            constexpr double kLambda1 = 0.000001;
            constexpr double kLambda2 = 0.000001;
            constexpr double kEps = 0.1;
            for (size_t i = 0; i < q_.size(); ++i) {
                double g = kLambda1 * err * x_[i];
                q_[i] -= 1.0 / sqrt(sg_[i] + kEps) * g;
                sg_[i] += sqr(g);
                q_[i] -= kLambda2 * q_[i];
            }
            cout << OUT(err) << OUTLN(q_);
        }
    }

    double mul() const {
        double res = 0;
        for (size_t i = 0; i < q_.size(); ++i) {
            res += q_[i] * x_[i];
        }
        return res;
    }

    double predict() {
        if (points_.empty()) {
            return 0;
        }

        while ((points_.size() < nDim_ + nSteps_) || (nIt_ < 20)) {
            return points_.back();
        }

        for (size_t i = 0; i < nDim_; ++i) {
            x_[i + 1] = points_[i + nSteps_];
        }

        return mul();
    }

   private:
    size_t nDim_;
    size_t nSteps_;
    size_t nIt_;
    deque<double> points_;
    DoubleVector q_;
    DoubleVector x_;
    DoubleVector sg_;
};

DoubleVector readTimeSeries(const std::string& filename) {
    IFStream fIn(kAaplTimeSeries);
    DoubleVector ts(kN);
    for (size_t i = 0; i < kN; ++i) {
        ts[i] = stod(split(fIn.readLine(), '\t')[1]);
    }

    return ts;
}

void fftTimeSeries() {
    Timer tFFT("FFT");
    auto ts = readTimeSeries(kAaplTimeSeries);

    auto avg = average(ts);
    DoubleVector nts(ts.size());
    for (size_t i = 0; i < ts.size(); ++i) {
        nts[i] = ts[i] - avg;
    }

    static const size_t kHorizon = nts.size() / 2;
    auto ctsSize = nts.size() - kHorizon;
    fft::ComplexVector cts(ctsSize);
    for (size_t i = 0; i < cts.size(); ++i) {
        cts[i] = complex<double>(nts[i], 0);
    }
    fft::transformNotNorm(cts);

    arma::cx_vec acts;
    acts.zeros(cts.size());
    for (size_t i = 0; i < cts.size(); ++i) {
        acts[i] = complex<double>(nts[i], 0);
    }
    acts = arma::fft(acts);

    for (size_t i = 0; i < cts.size(); ++i) {
        cout << i << "\t" << cts[i] << "\t" << abs(cts[i]) << "\t" << acts[i] << endl;
    }

    auto restored = cts;
    fft::inverseTransform(restored);

    fft::ComplexVector extended(cts.size() + kHorizon);
    for (size_t i = 0; i < extended.size(); ++i) {
        for (size_t j = 0; j < 7; ++j) {
            extended[i] += cts[j] * std::exp(complex<double>(0, (2.0 * M_PI * i * j) / cts.size()));
        }
    }
    for (auto& x: extended) {
        x /= cts.size();
    }

    OFStream fOut(kAaplTimeSeries + ".fft.tsv");
    for (size_t i = 0; i < extended.size(); ++i) {
        complex<double> r;
        double p = 0;
        if (i < restored.size()) {
            r = restored[i];
        }
        if (i < nts.size()) {
            p = nts[i];
        }

        fOut << i << "\t" << p << "\t" << r << "\t" << extended[i] << "\t" << extended[i].real() << endl;
    }
}

void predict() {
    Timer tPreict("Predict");
    auto ts = readTimeSeries(kAaplTimeSeries);

    // ts[i] = static_cast<double>(i) + 10*sin(i);
    // ts[i] = 10.0*sin((double)i/100.0);

    constexpr size_t kPoints = 200;
    constexpr size_t kDim = 32;
    constexpr size_t kR = 3;
    constexpr size_t kSteps = 10;
    SpectralPredictor sp(kPoints, kDim, kR);
    LinearPredictor lp(kPoints, kDim);
    SGDPredictor sgd(kDim, kSteps);

    double errorSpectral = 0;
    double errorLinear = 0;
    double errorSGD = 0;
    double errorLast = 0;

    for (size_t i = 0; i < ts.size(); ++i) {
        sp.addPoint(ts[i]);
        lp.addPoint(ts[i]);
        sgd.addPoint(ts[i]);
        if (i != 0 && i + kSteps < ts.size()) {
            errorSpectral += sqr(sp.predict(kSteps) - ts[i + kSteps]);
            errorLinear += sqr(lp.predict(kSteps) - ts[i + kSteps]);
            errorSGD += sqr(sgd.predict() - ts[i + kSteps]);
            errorLast += sqr(ts[i] - ts[i + kSteps]);
        }
    }

    LOG(INFO) << OUT(kR) << OUT(errorSpectral) << OUT(errorLinear) << OUT(errorLast) << OUT(errorSGD);
}

struct StockStat {
    double sumVolume_{0};
    double sumPrice_{0};
    size_t count_{0};

    double avgPrice() const {
        if (count_) {
            return sumPrice_ / count_;
        } else {
            return 0.;
        }
    }
};

using StockStats = unordered_map<string, StockStat>;

struct StockStatReutersParserCallback : public IReutersParserCallback {
    void onTrade(const std::string& ric, const DateTime& dt, double price, double volume, const BidAsk& bidask,
                 int index) override {
        if (volume > 0) {
            auto& stat = data_[ric];
            stat.sumVolume_ += volume;
            stat.sumPrice_ += price;
            ++stat.count_;
        }
    }

    const StockStat& get(const std::string& ric) const { return data_.find(ric)->second; }

    const StockStats& get() const { return data_; }

    StockStats data_;

    void save(const string& filename) const {
        OFStream ofs(filename);
        for (const auto& p: data_) {
            ofs << p.first << "\t" << p.second.sumVolume_ << "\t" << p.second.sumPrice_ << "\t" << p.second.count_ << endl;
        }
    }

    void load(const string& filename) {
        IFStream ifs(filename);
        while (ifs) {
            string ric;
            ifs >> ric;
            auto& stat = data_[ric];
            ifs >> stat.sumVolume_ >> stat.sumPrice_ >> stat.count_;
        }
    }
};

using StocksFeatures = unordered_map<string, StockFeatures>;

static constexpr double kTimeQuant = 1.0 / kQuants;

struct StockFeaturizerReutersParserCallback : public IReutersParserCallback {
    StockFeaturizerReutersParserCallback() : goodTick_(kQuants) {}

    static int getQuant(const DateTime& dt) {
        auto result = static_cast<int>(dt.time_.time_ / kTimeQuant);
        ENFORCE(result < static_cast<int>(kQuants));
        ENFORCE(result >= 0);
        return result;
    }

    DoubleVector& getFeatures(const std::string& ric, const DateTime& dt) {
        auto& stockFeatures = features_[ric];
        if (stockFeatures.empty()) {
            stockFeatures.resize(kQuants, DoubleVector(kDNNFeatures));
        }
        auto quant = getQuant(dt);
        return stockFeatures[quant];
    }

    void onQuote(const std::string& ric, const DateTime& dt, const BidAsk& bidask) override {
        if (bidask.open_) {
            goodTick_[getQuant(dt)] = true;
            auto& features = getFeatures(ric, dt);
            features[FI_BID] = bidask.bid_;
            features[FI_ASK] = bidask.ask_;
            features[FI_BIDSIZE] = bidask.bidSize_;
            features[FI_ASKSIZE] = bidask.askSize_;
        }
    }

    void onTrade(const std::string& ric, const DateTime& dt, double price, double volume, const BidAsk& bidask,
                 int index) override {
        auto& features = getFeatures(ric, dt);
        features[FI_VOLUME] += volume;
        features[FI_PRICE] += price;
        features[FI_TRADES] += 1.0;
        features[FI_LAST] = price;
    }

    void normalize(size_t featureIndex) {
        double sum = 0;
        double sum2 = 0;
        size_t count = 0;
        for (auto& stockPair : features_) {
            for (auto& features : stockPair.second) {
                sum += features[featureIndex];
                sum2 += sqr(features[featureIndex]);
                ++count;
            }
        }
        double mean = 0.0;
        double sigma = 1.0;
        if (count > 1) {
            mean = sum / count;
            sigma = sqrt((sum2 - (sqr(sum) / count)) / (count - 1));
        }
        for (auto& stockPair : features_) {
            for (auto& features : stockPair.second) {
                features[featureIndex] = (features[featureIndex] - mean) / sigma;
            }
        }
        LOG(INFO) << OUT(featureIndex) << OUT(mean) << OUT(sigma);
    }

    void normalizeFastSigmoid(size_t featureIndex) {
        for (auto& stockPair : features_) {
            for (auto& features : stockPair.second) {
                auto& f = features[featureIndex];
                f = f / (1.0 + abs(f));
            }
        }
    }

    void finish(const StockStats& ss) {
        for (auto& stockPair : features_) {
            auto toSd = ss.find(stockPair.first);
            if (toSd == ss.end()) {
                continue;
            }
            const auto& sd = toSd->second;
            double lastPrice = 0;
            double last = 0;
            double lastBid = 0;
            double lastAsk = 0;
            double lastBidSize = 0;
            double lastAskSize = 0;
            size_t index = 0;
            for (auto& features: stockPair.second) {
                if (features[FI_TRADES]) {
                    features[FI_PRICE] /= features[FI_TRADES];
                }
                if (sd.sumVolume_) {
                    features[FI_VOLUME] /= sd.sumVolume_;
                }
                auto avgPrice = sd.avgPrice();
                if (avgPrice) {
                    features[FI_PRICE] /= avgPrice;
                    features[FI_LAST] /= avgPrice;
                    features[FI_BID] /= avgPrice;
                    features[FI_ASK] /= avgPrice;
                }
                /*
                */
                features[FI_TRADES] = log(1.0 + features[FI_TRADES]);
                features[FI_ASKSIZE] = log(1.0 + features[FI_ASKSIZE]);
                features[FI_BIDSIZE] = log(1.0 + features[FI_BIDSIZE]);

                auto updateLast = [](double& featureVar, double& lastVar) {
                    if (featureVar) {
                        lastVar = featureVar;
                    } else {
                        featureVar = lastVar;
                    }
                };

                updateLast(features[FI_PRICE], lastPrice);
                updateLast(features[FI_LAST], last);
                updateLast(features[FI_BID], lastBid);
                updateLast(features[FI_ASK], lastAsk);
                updateLast(features[FI_BIDSIZE], lastBidSize);
                updateLast(features[FI_ASKSIZE], lastAskSize);

                features[FI_TDF] = static_cast<double>(index) / kQuants;

                if ((features[FI_BID] != 0) && (features[FI_ASK] != 0)) {
                    features[FI_CURRENT_PRICE] = (features[FI_ASK] + features[FI_BID]) / 2.0;
                } else if (features[FI_LAST] != -1) {
                    features[FI_CURRENT_PRICE] = features[FI_LAST];
                }

                ++index;
            }
        }
        for (size_t iFeature = 0; iFeature < kDNNFeatures; ++iFeature) {
            normalize(iFeature);
            // normalizeFastSigmoid(iFeature);
        }

        for (auto& stockPair : features_) {
            auto toSd = ss.find(stockPair.first);
            if (toSd == ss.end()) {
                continue;
            }
            for (auto& features: stockPair.second) {
                features[FI_INTERCEPT] = 1.0;
            }
        }
        /*
        */
    }

    void save(const string& filename) const {
        OFStream ofs(filename);
        for (const auto& p: features_) {
            ofs << p.first;
            for (size_t i = 0; i < kQuants; ++i) {
                ofs << "\t";
                saveVector(ofs, p.second[i]);
            }
            ofs << endl;
        }
    }

    void load(const string& filename) {
        IFStream ifs(filename);
        while (ifs) {
            string ric;
            ifs >> ric;
            if (ric.empty()) {
                break;
            }
            auto& fs = features_[ric];
            fs.resize(kQuants);
            for (size_t i = 0; i < kQuants; ++i) {
                fs[i] = loadVector<double>(ifs);
                ENFORCE_EQ(fs[i].size(), kDNNFeatures);
            }
        }
    }

    StocksFeatures features_;
    BoolVector goodTick_;
};

static const string kDNNStatFilename = "dnnStat.tsv";
static const string kDNNFeaturesFilename = "dnnFeatures.tsv";

void dnn() {
    Timer tTotal("DNN");
    StockFeaturizerReutersParserCallback featurizerCallback;
    StockStatReutersParserCallback statCallback;
    if (FLAGS_parse_dnn) {
        parseReuters(kFilename, statCallback);
        statCallback.save(kDNNStatFilename);
        parseReuters(kFilename, featurizerCallback);
        featurizerCallback.finish(statCallback.get());
        featurizerCallback.save(kDNNFeaturesFilename);
    } else {
        statCallback.load(kDNNStatFilename);
        featurizerCallback.load(kDNNFeaturesFilename);
    }
    auto stockStats = statCallback.get();
    const auto& features = featurizerCallback.features_;
    const auto& goodTicks = featurizerCallback.goodTick_;
    for (size_t i = 1; i < goodTicks.size(); ++i) {
        if (goodTicks[i - 1] != goodTicks[i]) {
            LOG(INFO) << OUT(i - 1) << OUT(goodTicks[i - 1]) << OUT(i) << OUT(goodTicks[i]);
        }
    }
    LOG(INFO) << OUT(features.size());

    static constexpr size_t kFirstTick = 870;
    static constexpr size_t kLastTick = 1260;

    auto genFeatures = [](const StockFeatures& sfeatures, size_t offset) {
        DoubleVector dnnFeatures;
        dnnFeatures.reserve(kDNNWindow * kDNNFeatures);
        for (size_t j = offset; j < offset + kDNNWindow; ++j) {
            for (size_t k = 0; k < kDNNFeatures; ++k) {
                auto f = sfeatures[j][k];
                ENFORCE(!isnan(f));
                dnnFeatures.emplace_back(f);
            }
        }
        return dnnFeatures;
    };

    auto genRet = [](const StockFeatures& sfeatures, int offset) {
        auto getPrice = [](const DoubleVector& slice) {
            return slice[FI_CURRENT_PRICE];
        };

        int futureIndex = offset + kDNNWindow + kDNNHorizon - 1;
        ENFORCE(futureIndex >= 0 && futureIndex < static_cast<int>(sfeatures.size()));

        const auto& future = sfeatures[futureIndex];
        auto futureResult = getPrice(future);
        ENFORCE(!isnan(futureResult));

        int index = offset + kDNNWindow - 1;
        if (index < 0) {
            return make_pair(0.0, false);
        }

        const auto& now = sfeatures[index];
        auto result = getPrice(now);
        ENFORCE(!isnan(result));

        if (!result) {
            return make_pair(0.0, false);
        }

        return make_pair(futureResult / result - 1.0, true);
    };

    auto train = [](const string& stock) {
        return (hash<string>()(stock) % 5) != 4;
    };

    auto stocks = keys(features);
    size_t samples = 0;
    for (const auto& stock : stocks) {
        if (!train(stock)) {
            continue;
        }

        if (!stockStats.count(stock)) {
            continue;
        }

        for (size_t i = kFirstTick; i + kDNNWindow + kDNNHorizon < kLastTick; ++i) {
            ++samples;
        }
    }
    LOG(INFO) << "Training samples: " << samples;

    DNNModelTrainer trainer(FLAGS_learning_rate, FLAGS_regularization, samples);
    TimerTracker tt;
    for (int iEpoch = 0; iEpoch < FLAGS_epochs; ++iEpoch) {
        auto modelStat = [&](auto& model) {

            double trainError = 0;
            double testError = 0;
            double testErrorBaseline1 = 0;
            double testErrorBaseline2 = 0;
            size_t testCount = 0;
            size_t trainCount = 0;
            double sum = 0;
            double sum2 = 0;
            for (const auto& stockPair : features) {
                if (!stockStats.count(stockPair.first)) {
                    continue;
                }

                const auto& sfeatures = stockPair.second;
                for (int i = kFirstTick; i + kDNNWindow + kDNNHorizon < kLastTick; ++i) {
                    auto dnnFeatures = genFeatures(sfeatures, i);
                    auto futureRet = genRet(sfeatures, i);
                    auto ret = genRet(sfeatures, i - kDNNHorizon);

                    if (!ret.second || !futureRet.second) {
                        continue;
                    }

                    auto prediction = model->predict(dnnFeatures);
                    auto sampleError = prediction - futureRet.first;
                    if (false && abs(sampleError) > 0.1) {
                        LOG_EVERY_MS(INFO, 1000) << OUT(dnnFeatures) << OUT(futureRet.first) << OUT(prediction)
                                                 << OUT(stockPair.first) << OUT(i + kDNNWindow + kDNNHorizon - 1);
                    }
                    if (!train(stockPair.first)) {
                        testError += sqr(sampleError);
                        testErrorBaseline1 += sqr(futureRet.first - ret.first);
                        testErrorBaseline2 += sqr(futureRet.first);
                        sum += futureRet.first;
                        sum2 += sqr(futureRet.first);
                        ++testCount;
                    } else {
                        trainError += sqr(sampleError);
                        ++trainCount;
                    }
                }
            }

            double pTrainError = sqrt(trainError / trainCount);
            double pTestError = sqrt(testError / testCount);
            double pTestBaseline1 = sqrt(testErrorBaseline1 / testCount);
            double pTestBaseline2 = sqrt(testErrorBaseline2 / testCount);
            double mean = sum / testCount;
            double variance = sum2 - 2.0 * mean * sum + testCount * sqr(mean);
            cout << "Epoch: " << iEpoch << ", test error: " << pTestError << ", baseline1 error: " << pTestBaseline1
                 << ", baseline2 error: " << pTestBaseline2 << ", train error: " << pTrainError
                 << ", test/train: " << pTestError / pTrainError << ", ratio1: " << pTestError / pTestBaseline1
                 << ", ratio2: " << pTestError / pTestBaseline2 << ", samples: " << testCount << ", mean: " << mean
                 << ", r2: " << 1.0 - (testError / variance) << ", elapsed: " << tt.diffAndReset() << endl;
        };

        auto oldModel = trainer.getModel();
        trainer.slowdown();

        shuffle(stocks);
        for (const auto& stock : stocks) {
            if (!train(stock)) {
                continue;
            }

            if (!stockStats.count(stock)) {
                continue;
            }

            // LOG_EVERY_MS(INFO, 1000) << OUT(stockPair.first);
            vector<DoubleVector> feats;
            DoubleVector label;
            const auto& sfeatures = features.find(stock)->second;
            for (size_t i = kFirstTick; i + kDNNWindow + kDNNHorizon < kLastTick; ++i) {
                auto dnnFeatures = genFeatures(sfeatures, i);
                auto ret = genRet(sfeatures, i);
                if (!ret.second) {
                    continue;
                }
                feats.emplace_back(std::move(dnnFeatures));
                label.emplace_back(ret.first);
                // LOG_EVERY_MS(INFO, 1000) << OUT(dnnFeatures) << OUT(ret);
            }
            trainer.train(feats, label);
        }

        auto model = trainer.getModel();
        model->save("dnn");
        model->saveJson("dnn.json");
        // modelStat(oldModel);
        modelStat(model);
    }
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    if (FLAGS_generate) {
        generate();
    }
    if (FLAGS_linear || FLAGS_fft) {
        produceTimeSeries();
    }
    if (FLAGS_fft) {
        fftTimeSeries();
    }
    if (FLAGS_linear) {
        predict();
    }
    dnn();
    return 0;
}
