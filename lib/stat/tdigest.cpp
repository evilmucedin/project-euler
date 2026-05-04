#include "tdigest.h"

#include "glog/logging.h"

size_t TDigest::checkWeights(const std::vector<Centroid>& sorted, Value total) {
    size_t badWeight = 0;
    auto k1 = 0.0;
    auto q = 0.0;
    for (auto iter = sorted.cbegin(); iter != sorted.cend(); iter++) {
        auto w = iter->weight();
        auto dq = w / total;
        auto k2 = integratedLocation(q + dq);
        if (k2 - k1 > 1 && w != 1) {
            LOG(WARNING) << "Oversize centroid at " << std::distance(sorted.cbegin(), iter) << " k1 " << k1 << " k2 "
                         << k2 << " dk " << (k2 - k1) << " w " << w << " q " << q;
            badWeight++;
        }
        if (k2 - k1 > 1.5 && w != 1) {
            LOG(ERROR) << "Egregiously Oversize centroid at " << std::distance(sorted.cbegin(), iter) << " k1 " << k1
                       << " k2 " << k2 << " dk " << (k2 - k1) << " w " << w << " q " << q;
            badWeight++;
        }
        q += dq;
        k1 = k2;
    }

    return badWeight;
}

