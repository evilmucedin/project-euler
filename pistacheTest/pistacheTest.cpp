#include "lib/header.h"

#include "pistache/client.h"

int main() {
    Pistache::Http::Experimental::Client client;

    auto opts = Pistache::Http::Experimental::Client::options().threads(1).maxConnectionsPerHost(8);
    client.init(opts);

    vector<Pistache::Async::Promise<Pistache::Http::Response>> responses;

    auto resp = client.get("pelevin.ru/").send();
    resp.then([&](Pistache::Http::Response response) {
        cout << response.code() << endl;
        cout << response.body() << endl;
        for (const auto& h: response.headers().list()) {
            cout << "\t" << h->name() << "\t";
            h->write(cout);
            cout << endl;
        }
    }, Pistache::Async::IgnoreException);
    responses.emplace_back(std::move(resp));

    auto sync = Pistache::Async::whenAll(responses.begin(), responses.end());
    Pistache::Async::Barrier<std::vector<Pistache::Http::Response>> barrier(sync);

    barrier.wait_for(std::chrono::seconds(5));

    client.shutdown();

    return 0;
}
