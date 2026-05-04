#pragma once

#include "lib/header.h"

struct Order {
    bool isBid_;
    double price_;
    size_t quantity_;
    int id_;
    int playerId_;
    int creationTime_;

    Order();
    Order(bool isBid, double price, size_t quantity);

    bool operator==(const Order& order) const;
    bool operator<(const Order& order) const;
};

ostream& operator<<(ostream& s, const Order& order);

using Orders = vector<Order>;

struct OrderBook {
    void addOrder(const Order& order);
    void deleteOrder(const Order& order);

    Orders cross();

    Orders orders_;
    size_t totalBid_{};
    size_t totalAsk_{};
};
