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
#include "lib/io/zstream.h"
#include "lib/math.h"
#include "lib/matrix.h"
#include "lib/random.h"
#include "lib/string.h"
#include "lib/timer.h"

DEFINE_bool(generate, false, "parse raw Reuters data");

using namespace Eigen;

// static const string kStock = "AAPL.O";
// static const string kStock = "YNDX.O";
static const string kStock = "AAAP.O";
static const string kAaplFilename = kStock + ".tsv";
static const string kAaplTimeSeries = kStock + ".ts";
static const string kHistogram = "histogram.tsv";

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
            for (auto percentile :
                 {0.0, 0.683772233983162, 0.9, 0.9683772233983162, 0.99, 0.9968377223398316, 0.999, 0.9996837722339832, 0.9999, 0.9999683772233983, 0.99999, 0.9999968377223398}) {
                fOut << p(percentile);
            }
            fOut << endl;
        }
    }

    unordered_map<string, DoubleVector> storage_;
};

struct BidAsk {
    double bid_;
    double ask_;
    double quoteTime_;
};

void parseReuters() {
    Timer tTotal("Parse Reuters");
    auto fIn = make_shared<IFStream>(homeDir() + "/Downloads/NSQ-2017-11-28-MARKETPRICE-Data-1-of-1.csv.gz", std::ifstream::binary);
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
    OFStream fOut(kAaplFilename);
    std::unordered_map<std::string, BidAsk> bidask;
    OFStream fPriceLevelsOut("priceLevels.tsv");
    OFStream fSubset("subset.csv");
    Histogramer h;
    while (reader.readLine()) {
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
                    double spread = bidask[ric].bid_ - bidask[ric].ask_;
                    double bam = (bidask[ric].bid_ + bidask[ric].ask_) / 2;
                    double priceLevel = 0;
                    if (spread) {
                        priceLevel = (price - bam) / spread;
                    }
                    fPriceLevelsOut << ric << "\t" << timestamp << "\t" << dateTime.time_.time_ << "\t" << bidask[ric].quoteTime_ << "\t" << bidask[ric].bid_ << "\t"
                                    << bidask[ric].ask_ << "\t" << price << "\t" << priceLevel << "\t" << index << std::endl;
                } else if (fidName == "TRDVOL_1") {
                    volume = reader.getDouble(iFidValue);
                }
            }
            if (0 != volume) {
                h.add(ric, volume);
            }
            if (ric == kStock) {
                if (volume > 0) {
                    fOut << dateTime.time_.time_ << "\t" << price << "\t" << volume << endl;
                }
            }
            LOG_EVERY_MS(INFO, 1000) << OUT(ric) << OUT(dateTime.str()) << OUT(price) << OUT(volume);
        } else if (recordType == "QUOTE") {
            auto ric = reader.get(iRic);
            if (ric == kStock) {
                fSubset << reader.line() << std::endl;
            }
            auto timestamp = reader.get(iDateTime);
            DateTime dateTime(timestamp);
            for (int iFid = 0; iFid < nFids; ++iFid) {
                reader.readLine();
                if (ric == kStock) {
                    fSubset << reader.line() << std::endl;
                }
                auto fidName = reader.get(iFidName);
                if (fidName == "BID") {
                    if (!reader.empty(iFidValue)) {
                        bidask[ric].bid_ = reader.getDouble(iFidValue);
                        bidask[ric].quoteTime_ = dateTime.time_.time_;
                    }
                } else if (fidName == "ASK") {
                    if (!reader.empty(iFidValue)) {
                        bidask[ric].ask_ = reader.getDouble(iFidValue);
                        bidask[ric].quoteTime_ = dateTime.time_.time_;
                    }
                }
            }
        } else {
            reader.skipLines(nFids);
        }
    }
    LOG(INFO) << OUT(reader.line());
    h.dump();
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

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    if (FLAGS_generate) {
        parseReuters();
    }
    produceTimeSeries();
    fftTimeSeries();
    predict();
    return 0;
}
