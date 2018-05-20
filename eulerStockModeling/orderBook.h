#pragma once

struct Order {
    bool isBid_;
    double price_;
    double quantity_;

    bool operator<(const Order& order) const;
};

struct OrderBook {

};
