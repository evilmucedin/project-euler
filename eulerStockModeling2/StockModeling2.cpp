#include "lib/header.h"
#include "lib/random.h"

static constexpr size_t N_PLAYERS = 32;
static constexpr size_t N_IT = 1000000000;

struct Player {
    double cash_{};
    int shares_{0};
};

using Players = vector<Player>;

int main() {
    Players players(N_PLAYERS);

    for (auto& player: players) {
        player.cash_ = randAB<double>(10, 100);
        player.shares_ = randAB<int>(0, 10);
    }

    double stockPrice = 100.0;

    for (size_t it = 0; it < N_IT; ++it) {
        double bestBid = 0;
        int bestBidIndex = -1;
        double bestAsk = 1000000;
        int  bestAskIndex = -1;
        for (size_t index = 0; index < N_PLAYERS; ++index) {
            auto& player = players[index];
            if (oneIn(2)) {
                double bid = stockPrice * randAB<double>(0.91, 1.11);
                if (bid <= player.cash_) {
                    if (bid > bestBid) {
                        bestBid = bid;
                        bestBidIndex = index;
                    }
                }
            } else {
                if (player.shares_) {
                    double ask = stockPrice * randAB<double>(0.9, 1.1);
                    if (ask < bestAsk) {
                        bestAsk = ask;
                        bestAskIndex = index;
                    }
                }
            }
        }

        if ( (bestBid >= bestAsk) && (bestBidIndex != -1 && bestAskIndex != -1) ) {
            stockPrice = (bestAsk + bestBid) / 2.0;
            players[bestBidIndex].cash_ -= stockPrice;
            ++players[bestBidIndex].shares_;
            players[bestAskIndex].cash_ += stockPrice;
            --players[bestAskIndex].shares_;
        } else {
            stockPrice *= 0.99;
        }

        double dividend = randAB<double>(0.01, 0.1);
        for (auto& player: players) {
            player.cash_ += player.shares_*dividend;
            player.cash_ -= min(player.cash_, 0.1 + 0.01*sqrt(player.cash_));
        }
        stockPrice += dividend;

        // double delta = static_cast<double>(balance)/10.0;
        // double mean = 0.1;
        // stockPrice = max(1.0, stockPrice + randAB<double>(-mean + delta, mean + delta));

        auto money = [&](size_t i) {
            return players[i].shares_*stockPrice + players[i].cash_;
        };

        if ((it % (N_IT / 100)) == 0) {
            double sum = 0.0;
            for (size_t i = 0; i < N_PLAYERS; ++i) {
                sum += money(i);
            }
            cout << "sp: " << stockPrice << " sum: " << sum;
            for (size_t i = 0; i < N_PLAYERS; ++i) {
                double wealth = money(i)/sum;
                if (wealth > 0.0001) {
                    cout << " " << i << ":" << wealth;
                }
            }
            cout << endl;
        }
    }

    return 0;
}
