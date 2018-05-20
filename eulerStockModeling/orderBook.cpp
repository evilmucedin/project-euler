#include "orderBook.h"

#include "lib/random.h"

Order::Order() { id_ = randInt(); }

Order::Order(bool isBid, double price, size_t quantity) : isBid_(isBid), price_(price), quantity_(quantity) {
    id_ = randInt();
}

bool Order::operator==(const Order& order) const { return id_ == order.id_; }

bool Order::operator<(const Order& order) const { return price_ < order.price_; }

ostream& operator<<(ostream& s, const Order& order) {
    s << "{" << ((order.isBid_) ? "bid" : "ask") << ", " << order.price_ << ", " << order.quantity_ << "}";
    return s;
}

void OrderBook::addOrder(const Order& order) {
    if (order.isBid_) {
        totalBid_ += order.quantity_;
    } else {
        totalAsk_ += order.quantity_;
    }
    orders_.emplace_back(order);
}

void OrderBook::deleteOrder(const Order& order) {
    auto toOrder = find(orders_.begin(), orders_.end(), order);
    assert(toOrder != orders_.end());
    orders_.erase(toOrder);
    if (order.isBid_) {
        totalBid_ -= order.quantity_;
    } else {
        totalAsk_ -= order.quantity_;
    }
}

Orders OrderBook::cross() {
    sort(orders_);
    size_t indicativeQuantity = 0;
    size_t currentBidQuantity = totalBid_;
    size_t currentAskQuantity = 0;
    double indicativePrice = 1e9;
    for (const auto& o : orders_) {
        if (o.isBid_) {
            currentBidQuantity -= o.quantity_;
        } else {
            currentAskQuantity += o.quantity_;
        }
        auto quantity = min(currentBidQuantity, currentAskQuantity);
        if (quantity > indicativeQuantity) {
            indicativePrice = o.price_;
            indicativeQuantity = o.quantity_;
        }
    }

    Orders result;

    for (auto toOrder = orders_.begin(); toOrder != orders_.end();) {
        bool include = false;
        if (toOrder->isBid_) {
            if (toOrder->price_ >= indicativePrice) {
                include = true;
            }
        } else {
            if (toOrder->price_ <= indicativePrice) {
                include = true;
            }
        }

        if (include) {
            if (toOrder->quantity_ <= indicativeQuantity) {
                indicativeQuantity -= toOrder->quantity_;
                result.emplace_back(*toOrder);
                orders_.erase(toOrder);
            } else {
                indicativeQuantity = 0;
                toOrder->quantity_ -= indicativeQuantity;
                ++toOrder;
            }
        } else {
            ++toOrder;
        }

        if (0 == indicativeQuantity) {
            break;
        }
    }

    return result;
}
