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

class FileInputStream;

class IFStreamProgressable : public IProgressable {
   public:
    IFStreamProgressable(std::weak_ptr<FileInputStream> stream);
    double progress() override;

   private:
    std::weak_ptr<FileInputStream> stream_;
    size_t size_;
};
