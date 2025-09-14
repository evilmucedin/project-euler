#include <set>
#include <string>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "lib/header.h"
#include "lib/init.h"
#include "lib/ml/dataframe.h"
#include "lib/random.h"
#include "lib/stat.h"
#include "lib/thread-pool/threadPool.h"
#include "lib/timer.h"

using namespace std;

DEFINE_string(first_date, "", "First modeling date");
DEFINE_bool(additive_sortino, true, "subtract risk");
DEFINE_int32(iterations, 10, "nuimber of iterations");
DEFINE_string(mode, "optimize", "mode (optimize, optimize1)");
DEFINE_string(input, "portfolio input", "");
DEFINE_string(tickers, "tickers", "");
DEFINE_bool(stocks, false, "add stocks");
DEFINE_double(risk_weight, 0.05, "risk weight");
DEFINE_bool(decay, true, "decay");
DEFINE_double(concentration_risk_weight, 0.00, "concentration risk weight");


/*

static const StringVector etfs = {
    "FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "IWM",  "VUG",  "SPY",  "IVV",   "VOO",  "QQQ",
    "BND",   "FBND",  "HDV",   "VEU",   "VWO",   "FDHY",  "FDIS", "ONEQ", "VV",   "VB",    "HNDL", "WBII",
    "PCEF",  "FDIV",  "CEFS",  "YLD",   "INKM",  "IYLD",  "FCEF", "MLTI", "YYY",  "MDIV",  "HIPS", "CVY",
    "GYLD",  "VTI",   "VEA",   "IEFA",  "AGG",   "GLD",   "XLF",  "VNQ",  "LQD",  "SWPPX", "MGK",  "UNG",
    "OIH",   "XME",   "PFIX",  "VXX",   "EWZ",   "ILF",   "SCHE", "FBCG", "FQAL", "FLPSX", "FDRR", "FMAG",
    "FPRO",  "FBCV",  "FMIL",  "BITO",  "BITW",  "VBB",   "SFY",  "IJR",  "SCHD", "FTLS",  "FDHT", "FRNW",
    "FDRV",  "FCLD",  "FDIG",  "ARKK",  "ITA",   "PPA",   "XAR",  "USO",  "IEO",  "SPGP",  "IWY",  "SPYG",
};

static const StringVector stocks = {
    "GOOG", "MSFT", "T",  "NCLH", "AMZN", "META", "TSLA", "GME",  "AAPL",  "NVDA", "TSM",  "UNH", "JNJ", "V",
    "WMT",  "JPM",  "PG", "XOM",  "HD",   "CVX",  "PFE",  "COIN", "VWAGY", "LMT", "KHC", "NKE", "SBUX",
};

*/

int main() {
    return 0;
}
