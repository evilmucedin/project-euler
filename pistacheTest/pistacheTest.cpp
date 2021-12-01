#include "pistache/client.h"

int main() {
    Pistache::Http::Experimental::Client client;

    auto opts = Pistache::Http::Experimental::Client::options().threads(1).maxConnectionsPerHost(8);
    client.init(opts);

    return 0;
}
