#include "glog/logging.h"
#include "eigen/Dense"

#include "lib/datetime.h"
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

using namespace Eigen;

static const string kStock = "AAPL.O";
// static const string kStock = "YNDX.O";
static const string kAaplFilename = kStock + ".tsv";
static const string kAaplTimeSeries = kStock + ".ts";

void parseReuters() {
    Timer tTotal("Parse Reuters");
    auto fIn = make_shared<IFStream>(homeDir() + "/Downloads/NSQ-2017-11-28-MARKETPRICE-Data-1-of-1.csv.gz",
                                     std::ifstream::binary);
    auto zIn = make_shared<ZIStream>(fIn);
    CsvParser reader(zIn);
    reader.readHeader();
    const int iFIDNumber = reader.getIndex("Number of FIDs");
    const int iType = reader.getIndex("UpdateType/Action");
    const int iDateTime = reader.getIndex("Date-Time");
    const int iFidName = reader.getIndex("FID Name");
    const int iFidValue = reader.getIndex("FID Value");
    const int iRic = reader.getIndex("#RIC");
    OFStream fOut(kAaplFilename);
    while (reader.readLine()) {
        int nFids = reader.getInt(iFIDNumber);
        auto recordType = reader.get(iType);
        if (recordType == "TRADE") {
            auto ric = reader.get(iRic);
            DateTime dateTime(reader.get(iDateTime));
            double price = 0;
            double volume = 0;
            for (int iFid = 0; iFid < nFids; ++iFid) {
                reader.readLine();
                auto fidName = reader.get(iFidName);
                if (fidName == "TRDPRC_1") {
                    price = reader.getDouble(iFidValue);
                } else if (fidName == "TRDVOL_1") {
                    volume = reader.getDouble(iFidValue);
                }
            }
            if (ric == kStock) {
                if (volume > 0) {
                    fOut << dateTime.time_.time_ << "\t" << price << "\t" << volume << endl;
                }
            }
            LOG_EVERY_MS(INFO, 1000) << OUT(ric) << OUT(dateTime.str()) << OUT(price) << OUT(volume);
        } else {
            reader.skipLines(nFids);
        }
    }
    LOG(INFO) << OUT(reader.line());
}

constexpr size_t kN = 60*8;

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
        LOG(INFO) << OUTLN(esC.eigenvalues());
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

        auto b = linearRegression(points);
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

void predict() {
    Timer tPreict("Predict");
    IFStream fIn(kAaplTimeSeries);
    DoubleVector ts(kN);
    for (size_t i = 0; i < kN; ++i) {
        ts[i] = stod(split(fIn.readLine(), '\t')[1]);
        // ts[i] = static_cast<double>(i) + 10*sin(i);
        // ts[i] = 10.0*sin((double)i/100.0);
    }

    constexpr size_t kPoints = 50;
    constexpr size_t kDim = 32;
    constexpr size_t kR = 3;
    SpectralPredictor sp(kPoints, kDim, kR);
    LinearPredictor lp(kPoints, kDim);

    double errorSpectral = 0;
    double errorLinear = 0;
    double errorLast = 0;
    constexpr size_t kSteps = 10;

    for (size_t i = 0; i < ts.size(); ++i) {
        sp.addPoint(ts[i]);
        lp.addPoint(ts[i]);
        if (i != 0 && i + kSteps < ts.size()) {
            errorSpectral += sqr(sp.predict(kSteps) - ts[i + kSteps]);
            errorLinear += sqr(lp.predict(kSteps) - ts[i + kSteps]);
            errorLast += sqr(ts[i] - ts[i + kSteps]);
        }
    }

    LOG(INFO) << OUT(kR) << OUT(errorSpectral) << OUT(errorLinear) << OUT(errorLast);
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    // parseReuters();
    produceTimeSeries();
    predict();
    return 0;
}
