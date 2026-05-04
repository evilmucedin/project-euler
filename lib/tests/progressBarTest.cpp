#include "gtest/gtest.h"

#include "lib/progressBar.h"

class NaiveProgressable : public IProgressable {
public:
    double progress() override {
        result += 0.01;
        return result;
    }

private:
    double result{};
};

TEST(ProgresBar, Simple) {
    auto np = std::make_shared<NaiveProgressable>();
    ProgressBar::getInstance().setProgressable(np);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20s);
}
