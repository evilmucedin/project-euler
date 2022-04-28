#include "progressBar.h"

#include <unistd.h>

#include "lib/io/stream.h"

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
    fprintf(stderr, "\x1bk\x1b");
    fflush(stderr);
}
}

void ProgressBar::setProgressable(std::shared_ptr<IProgressable> progressable) {
    progressable_ = progressable;

    if (isTerminalStdout_ && !progressThread_) {
        progressThread_ = std::make_unique<std::thread>(ProgressBarThreadMethod);
    }
}

IFStreamProgressable::IFStreamProgressable(std::weak_ptr<FileInputStream> stream) : stream_(stream) {
    if (auto s = stream_.lock()) {
        auto sizeBefore = s->tell();
        assert(0 == sizeBefore);
        s->seekEnd(0);
        size_ = s->tellg();
        s->seekBegin(0);
    }
}

double IFStreamProgressable::progress() {
    if (auto s = stream_.lock()) {
        auto size = s->tell();
        return static_cast<double>(size) / size_;
    }
    return 1.1;
}

/*

case ${TERM} in
    xterm-color*)
        if [ "$TMUX" != "" ] ; then
            # user command to change default pane title on demand
            function title {
                TMUX_PANE_TITLE="$*";
            }

            # function that performs the title update (invoked as PROMPT_COMMAND)
            function update_title {
                s=${1:0:16};
                printf "\033k;%s\033" "${s:-$TMUX_PANE_TITLE}";
            }

            # default pane title is the name of the current process (i.e. 'bash')
            TMUX_PANE_TITLE=$(ps -o comm $$ | tail -1);

            # Reset title to the default before displaying the command prompt
            PROMPT_COMMAND=${PROMPT_COMMAND:+$PROMPT_COMMAND; }'update_title'

            # Update title before executing a command: set it to the command
            trap 'update_title "$BASH_COMMAND"' DEBUG
        fi

    ;;
esac


*/
