#include "glog/logging.h"

#include "lib/header.h"
#include "lib/io/file.h"
#include "lib/io/varlen.h"
#include "lib/noncopyable.h"
#include "lib/random.h"
#include "lib/spsc/blockSPSCQueue.h"
#include "lib/timer.h"

struct FileIterator {
    FileIterator(const std::string& s) : fIn_(s, "rb") {
        ASSERT(fIn_.opened());
        read();
    }

    void read() { eof_ = !maybeReadVarLen(fIn_, next_); }

    bool eof() const { return eof_; }

    int32_t next() const { return next_; }

    File fIn_;
    int32_t next_;
    bool eof_;
};

struct AsyncFileIterator : NonCopyable {
    AsyncFileIterator(const std::string& s)
        : t_([s, this]() {
              while (!inited_) {
              }
              File fIn(s, "rb");
              int32_t value;
              while (maybeReadVarLen(fIn, value)) {
                  q_.enqueue(value);
              }
              q_.enqueue(-1);
              q_.flush();
          }) {
        inited_ = true;
        read();
    }

    void read() {
        if (!eof_) {
            q_.dequeue(next_);
            if (next_ == -1) {
                eof_ = true;
            }
        }
    }

    int32_t next() const { return next_; }

    bool eof() const { return eof_; }

    ~AsyncFileIterator() { t_.join(); }

    std::atomic<bool> inited_{};
    std::thread t_;
    BlockSPSCQueue<int32_t> q_;
    int32_t next_;
    bool eof_{};
};

static const string kDirectory = "/home/denplusplus/Temp";

std::string genFilename(size_t i) { return kDirectory + "/" + to_string(i) + ".dat"; };

template <typename T, size_t M>
void merge(const std::string& filename) {
    Timer t("merge to " + filename);
    File fOut(filename, "wb");
    std::vector<std::shared_ptr<T>> its;
    for (size_t i = 0; i < M; ++i) {
        its.emplace_back(std::make_shared<T>(genFilename(i)));
    }

    size_t written = 0;
    while (true) {
        T* minIt = nullptr;
        int32_t min = 1 << 30;
        for (size_t i = 0; i < M; ++i) {
            if (!its[i]->eof()) {
                if (its[i]->next() < min) {
                    min = its[i]->next();
                    minIt = its[i].get();
                }
            }
        }
        if (minIt) {
            writeVarLen(fOut, min);
            minIt->read();
            ++written;
        } else {
            break;
        }
    }

    LOG(INFO) << "Merged to " << filename << ": " << written;
}

int main() {
    static const size_t N = 1000000;
    static const size_t M = 3;

    {
        Timer t("generation");
        for (size_t i = 0; i < M; ++i) {
            File fOut(genFilename(i), "wb");
            int32_t current = 0;
            for (size_t j = 0; j < N; ++j) {
                writeVarLen(fOut, current);
                current += dice(10);
            }
            LOG(INFO) << i << " " << fOut.tell();
        }
    }

    static const string kMerged1 = kDirectory + "/merged1.dat";
    {
        merge<FileIterator, M>(kMerged1);
    }

    static const string kMerged2 = kDirectory + "/merged2.dat";
    {
        merge<AsyncFileIterator, M>(kMerged2);
    }

    {
        Timer t("cleanup");
        for (size_t i = 0; i < M; ++i) {
            std::remove(genFilename(i).data());
        }
        std::remove(kMerged1.data());
        std::remove(kMerged2.data());
    }

    return 0;
}
