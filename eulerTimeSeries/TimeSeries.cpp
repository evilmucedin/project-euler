#include "glog/logging.h"
#include "eigen/Dense"

#include "lib/datetime.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/fstream.h"
#include "lib/io/zstream.h"
#include "lib/string.h"
#include "lib/timer.h"

using namespace Eigen;

static const string kAaplFilename = "aapl.tsv";
static const string kAaplTimeSeries = "aapl.ts";

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
            if (ric == "AAPL.O") {
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

constexpr size_t kN = 1000;

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
    for (size_t i = 0; i < kN; ++i) {
        fOut << i << "\t" << dv[i]/vol[i] << "\t" << vol[i] << "\t" << n[i] << endl;
    }
}

void predict() {
    Timer tPreict("Predict");
    IFStream fIn(kAaplTimeSeries);
    DoubleVector ts(kN);
    for (size_t i = 0; i < kN; ++i) {
        ts[i] = stod(split(fIn.readLine(), '\t')[1]);
    }
    constexpr size_t kDim = 32;
    constexpr size_t kPoints = kN/2;
    // Rows Cols
    Matrix<double, kDim, kPoints> x;
    for (size_t i = 0; i < kDim; ++i) {
        for (size_t j = 0; j < kPoints; ++j) {
            x(i, j) = ts[i + j];
        }
    }
    // cout << x << endl;

    auto c = x * x.transpose() / kPoints;
    SelfAdjointEigenSolver<MatrixXd> esC(c);
    // cout << c << endl;
    cout << esC.eigenvalues() << endl;
    auto v = esC.eigenvectors();
    //cout << "v:" << endl << v << endl;

    constexpr size_t kR = 8;

    Matrix<double, kDim - 1, kR> vStar;
    for (size_t i = 0; i < kDim - 1; ++i) {
        for (size_t j = 0; j < kR; ++j) {
            vStar(i, j) = v(i, kDim - 1 - j);
        }
    }
    // cout << "vStar:" << endl << vStar << endl;
    Matrix<double, 1, kR> vTau;
    for (size_t i = 0; i < kR; ++i) {
        vTau(0, i) = v(kDim - 1, kDim - 1 - i);
    }

    auto vTauVTauTM = vTau*vTau.transpose();
    auto vTauVTauT = vTauVTauTM(0, 0);
    auto predictionM = (vTau * vStar.transpose()) / (1 - vTauVTauT);
    Matrix<double, kDim - 1, 1> q;
    for (size_t i = 0; i < kDim - 1; ++i) {
        q(i, 0) = ts[(kPoints - 1 + kDim - 1) + i - (kDim - 1)];
    }
    cout << "q:" << endl << q << endl;
    cout << "prediction: " << endl << predictionM * q << endl;
    cout << "real: " << endl << ts[kPoints - 1 + kDim - 1] << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    // parseReuters();
    // produceTimeSeries();
    predict();
    return 0;
}
