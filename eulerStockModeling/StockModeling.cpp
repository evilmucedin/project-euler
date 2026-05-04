#include "glog/logging.h"

#include "lib/header.h"
#include "lib/io/csv.h"
#include "lib/io/utils.h"

#include "orderBook.h"

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

struct IPlayer {
    virtual ~IPlayer() = default;
    virtual vector<Order> getOrders();
    virtual void updateMoney(double value);
};

struct PlayerBase : public IPlayer {
    void updateMoney(double value) override { money_ += value; }

   private:
    double money_;
};

class Market {


};

int main() {
    Data data;

    return 0;
}
