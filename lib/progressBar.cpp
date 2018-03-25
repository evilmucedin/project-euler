#include "progressBar.h"

#include <unistd.h>

IProgressable::~IProgressable() = default;

ProgressBar& ProgressBar::getInstance() {
    static ProgressBar instance;
    return instance;
}

ProgressBar::ProgressBar() {
    isTerminalStdout_ = isatty(2) && getenv("TMUX");
}

ProgressBar::~ProgressBar() {
    progressable_.reset();
    if (progressThread_) {
        progressThread_->join();
    }
}

namespace {
void ProgressBarThreadMethod() {
    auto& instance = ProgressBar::getInstance();
    while (true) {
        if (auto p = instance.progressable_.lock()) {
            auto pr = p->progress();
            if (pr > 1) {
                break;
            }
            fprintf(stderr, "\x1bk%.2lf\x1b", (100.0 * pr));
            fflush(stderr);
        } else {
            break;
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
}
}

void ProgressBar::setProgressable(std::shared_ptr<IProgressable> progressable) {
    progressable_ = progressable;

    if (isTerminalStdout_ && !progressThread_) {
        progressThread_ = std::make_unique<std::thread>(ProgressBarThreadMethod);
    }
}
