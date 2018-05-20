#include "orderBook.h"

bool Order::operator<(const Order& order) const {
    return price_ < order.price_;
}
