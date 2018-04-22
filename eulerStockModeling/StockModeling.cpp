#include "glog/logging.h"

#include "lib/header.h"
#include "lib/io/csv.h"
#include "lib/io/utils.h"

struct Symbol {
    string name_;

    Symbol(string name) : name_(std::move(name)) {}
};

struct Data {
    Data() {
        CsvParser symbols(repoRoot() + "/eulerStockModeling/data/symbols.csv");
        while (symbols.readLine()) {
            symbols_.emplace_back(symbols.get(0));
        }
        LOG(INFO) << OUT(symbols_.size());
    }

    vector<Symbol> symbols_;
};

int main() {
    Data data;

    return 0;
}
