#pragma once

#include <memory>
#include <thread>

class IProgressable {
public:
    virtual ~IProgressable();

    virtual double progress() = 0;
};

class ProgressBar {
private:
    ProgressBar();
    ~ProgressBar();

public:
    static ProgressBar& getInstance();

    void setProgressable(std::shared_ptr<IProgressable> progressable);

    bool isTerminalStdout_;
    std::weak_ptr<IProgressable> progressable_;
    std::unique_ptr<std::thread> progressThread_;
};
