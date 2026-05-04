#include "lib/header.h"

#include "glog/logging.h"

#include "lib/io/utils.h"
#include "lib/string.h"

#include <memory>

namespace xgboost {

struct Node;
using PNode = std::shared_ptr<Node>;

struct Node {
    bool isLeaf_{};
    PNode left_{};
    PNode right_{};

    std::string factor_{};
    double threshold_{};
    double value_{};
};

struct Booster {
    Booster() {
    }

    Booster(PNode root) : root_(std::move(root)) {
    }

    PNode root_;
};

using PBooster = std::shared_ptr<Booster>;

struct Ranker {
    std::vector<PBooster> boosters_;

    PNode loadNodeFromLines(const std::vector<std::string>& lines, size_t& iLine) {
        LOG(INFO) << lines[iLine];
        const auto parts = split(lines[iLine], ':');
        assert(parts.size() == 2);
        auto result = std::make_shared<Node>();
        if (parts[1][0] == '[') {
            const auto substr = parts[1].substr(1, parts[1].find(']') - 2);
            const auto parts2 = split(substr, '<');
            assert(parts2.size() == 2);
            result->factor_ = parts2[0];
            result->threshold_ = std::atof(parts2[1].c_str());
            result->isLeaf_ = false;
            ++iLine;
            result->left_ = loadNodeFromLines(lines, iLine);
            result->right_ = loadNodeFromLines(lines, iLine);
        } else {
            assert(parts[1].substr(0, 5) == "leaf=");
            result->isLeaf_ = true;
            const auto parts2 = split(parts[1], '=');
            result->value_ = std::atof(parts2[1].c_str());
            ++iLine;
        }
        return result;
    }

    void loadFromLines(const std::vector<std::string>& lines) {
        boosters_.clear();
        size_t iLine = 0;
        while (iLine < lines.size()) {
            assert(hasSubstring(lines[iLine], "booster["));
            ++iLine;
            boosters_.emplace_back(std::make_shared<Booster>(loadNodeFromLines(lines, iLine)));
        }
    }
};

using PRanker = std::shared_ptr<Ranker>;

PRanker loadRanker(const std::string& filename) {
    auto result = std::make_shared<Ranker>();
    result->loadFromLines(loadAsLines(filename));
    return result;
}

}  // namespace xgboost

int main() {
    auto ranker = xgboost::loadRanker("/home/denplusplus/Temp/xgboost/demo/kaggle-higgs/higgs.raw.dump.txt");
    return 0;
}
