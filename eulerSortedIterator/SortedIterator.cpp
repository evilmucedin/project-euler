#include <glog/logging.h>

#include "lib/header.h"
#include "lib/random.h"
#include "lib/radixSort.h"
#include "lib/timer.h"

constexpr size_t kN = 100000;
constexpr size_t kM = 100;
constexpr size_t kD = 100;

vector<IntVector> data;

void genData() {
    Timer tGen("Gen");
    data.resize(kM);
    for (size_t i = 0; i < kM; ++i) {
        int next = dice(kD);
        data[i].resize(kN);
        for (size_t j = 0; j < kN; ++j) {
            data[i][j] = next;
            next += dice(kD);
        }
    }
}

struct IIterator {
    virtual bool has() const = 0;
    virtual int get() const = 0;
    virtual void next() = 0;
    virtual ~IIterator() = default;
};
using PIIterator = shared_ptr<IIterator>;
using PIIterators = vector<PIIterator>;

struct Iterator final : public IIterator {
    Iterator() : pos_(kN) {}

    Iterator(size_t index)
        : index_(index)
    {
    }

    bool has() const override {
        return pos_ < kN;
    }

    int get() const override {
        return data[index_][pos_];
    }

    void next() override {
        ++pos_;
    }

    bool operator<(const Iterator& it) const {
        return get() < it.get();
    }

    size_t index_;
    size_t pos_{0};
};
using PIterator = shared_ptr<Iterator>;
using PIterators = vector<PIterator>;

struct PIteratorsCmp {
    bool operator()(const Iterator* a, const Iterator* b) const{
        return *b < *a;
    }
};

struct HeapMergeIterator final : public IIterator {
    HeapMergeIterator(const PIterators& its) {
        its_.resize(its.size());
        for (size_t i = 0; i < its.size(); ++i) {
            its_[i] = its[i].get();
        }
        make_heap(its_.begin(), its_.end(), cmp_);
    }

    bool has() const override { return !its_.empty(); }

    int get() const override { return its_.front()->get(); }

    void next() override {
        its_.front()->next();
        if (!its_.front()->has()) {
            swap(its_.front(), its_.back());
            its_.pop_back();
        }
        if (!its_.empty()) {
            size_t index = 0;
            size_t limit = its_.size() / 2;
            while (index < limit) {
                size_t minIndex = 2 * index + 1;
                if (minIndex + 1 < its_.size()) {
                    if (cmp_(its_[minIndex], its_[minIndex + 1])) {
                        ++minIndex;
                    }
                }
                if (cmp_(its_[index], its_[minIndex])) {
                    swap(its_[index], its_[minIndex]);
                    index = minIndex;
                } else {
                    break;
                }
            }
        }
    }

    vector<Iterator*> its_;
    PIteratorsCmp cmp_;
};

struct BinaryMergeIterator final : public IIterator {
    BinaryMergeIterator(PIIterator a, PIIterator b) : a_(a), b_(b) {
        has_ = true;
        calc();
    }

    void calc() {
        bool aHas = a_->has();
        bool bHas = b_->has();
        if (aHas && bHas) {
            int aVal = a_->get();
            int bVal = b_->get();
            if (aVal < bVal) {
                val_ = aVal;
                a_->next();
            } else {
                val_ = bVal;
                b_->next();
            }
        } else if (aHas) {
            val_ = a_->get();
            a_->next();
        } else if (bHas) {
            val_ = b_->get();
            b_->next();
        } else {
            has_ = false;
        }
    }

    bool has() const override { return has_; }

    int get() const override { return val_; }

    void next() override { calc(); }

    PIIterator a_;
    PIIterator b_;
    int val_;
    bool has_;
};

static PIIterator itEof = make_shared<Iterator>();

struct BinaryTreeMergeIterator final : public IIterator {
    BinaryTreeMergeIterator(PIterators its) {
        PIIterators now(its.begin(), its.end());
        while (now.size() != 1) {
            PIIterators next;
            for (size_t i = 0; i < now.size(); i += 2) {
                PIIterator a = now[i];
                PIIterator b;
                if (i + 1 < now.size()) {
                    b = now[i + 1];
                } else {
                    b = itEof;
                }
                next.emplace_back(make_shared<BinaryMergeIterator>(a, b));
            }
            now = move(next);
        }
        root_ = now.front();
    }

    bool has() const { return root_->has(); }

    int get() const { return root_->get(); }

    void next() { root_->next(); }

    PIIterator root_;
};

struct BufferedIterator final : public IIterator {
    BufferedIterator(PIIterator it) : it_(it) { fillBuffer(); }

    void fillBuffer() {
        first_ = 0;
        length_ = 0;
        while (length_ < kBuffer && it_->has()) {
            buffer_[length_++] = it_->get();
            it_->next();
        }
    }

    bool has() const override { return first_ < length_; }

    int get() const override { return buffer_[first_]; }

    void next() override {
        ++first_;
        if (first_ >= length_) {
            fillBuffer();
        }
    }

    PIIterator it_;
    static constexpr int kBuffer = 128;
    int buffer_[kBuffer];
    int length_;
    int first_;
};

struct BufferedBinaryTreeMergeIterator final : public IIterator {
    BufferedBinaryTreeMergeIterator(PIterators its) {
        PIIterators now(its.begin(), its.end());
        while (now.size() != 1) {
            PIIterators next;
            for (size_t i = 0; i < now.size(); i += 2) {
                PIIterator a = now[i];
                PIIterator b;
                if (i + 1 < now.size()) {
                    b = now[i + 1];
                } else {
                    b = make_shared<Iterator>();
                }
                next.emplace_back(make_shared<BufferedIterator>(make_shared<BinaryMergeIterator>(a, b)));
            }
            now = move(next);
        }
        root_ = now.front();
    }

    bool has() const { return root_->has(); }

    int get() const { return root_->get(); }

    void next() { root_->next(); }

    PIIterator root_;
};

struct BufferedBinaryMergeIterator {
    BufferedBinaryMergeIterator() {
    }

    BufferedBinaryMergeIterator(Iterator* a, Iterator* b) {
        init(a, b);
    }

    void init(Iterator* a, Iterator* b) {
        ia_ = a;
        ib_ = b;
        raw_ = true;
        fillBuffer();
    }

    void init(BufferedBinaryMergeIterator* a, BufferedBinaryMergeIterator* b) {
        ba_ = a;
        bb_ = b;
        raw_ = false;
        fillBuffer();
    }

    bool hasInt() const {
        if (raw_) {
            return ia_->has() || ib_->has();
        } else {
            return ba_->has() || bb_->has();
        }
    }

    int getAndNextInt() const {
        int val;
        if (raw_) {
            bool aHas = ia_->has();
            bool bHas = ib_->has();
            if (aHas && bHas) {
                int aVal = ia_->get();
                int bVal = ib_->get();
                if (aVal < bVal) {
                    val = aVal;
                    ia_->next();
                } else {
                    val = bVal;
                    ib_->next();
                }
            } else if (aHas) {
                val = ia_->get();
                ia_->next();
            } else if (bHas) {
                val = ib_->get();
                ib_->next();
            }
        } else {
            bool aHas = ba_->has();
            bool bHas = bb_->has();
            if (aHas && bHas) {
                int aVal = ba_->get();
                int bVal = bb_->get();
                if (aVal < bVal) {
                    val = aVal;
                    ba_->next();
                } else {
                    val = bVal;
                    bb_->next();
                }
            } else if (aHas) {
                val = ba_->get();
                ba_->next();
            } else if (bHas) {
                val = bb_->get();
                bb_->next();
            }
        }
        return val;
    }

    void fillBuffer() {
        first_ = 0;
        length_ = 0;
        while (length_ < kBufferSize && hasInt()) {
            buffer_[length_++] = getAndNextInt();
        }
    }

    bool has() const { return first_ < length_; }

    int get() const { return buffer_[first_]; }

    void next() {
        ++first_;
        if (first_ >= length_) {
            fillBuffer();
        }
    }

    bool raw_;
    Iterator* ia_;
    Iterator* ib_;
    BufferedBinaryMergeIterator* ba_;
    BufferedBinaryMergeIterator* bb_;
    static constexpr int kBufferSize = 128;
    int buffer_[kBufferSize];
    int length_;
    int first_;
};

static Iterator eofIterator;
static BufferedBinaryMergeIterator bufferedEofIterator(&eofIterator, &eofIterator);

struct BufferedBinaryTreeMergeIterator2 : public IIterator {
    BufferedBinaryTreeMergeIterator2(PIterators its) {
        int size = 1;
        int n = its.size();
        while (n != 1) {
            size += (n + 1) / 2;
            n = (n + 1) / 2;
        }
        bits_.resize(size);

        size_t start = 0;
        size_t end = 0;
        size_t last = 0;

        for (size_t i = 0; i < its.size(); i += 2) {
            Iterator* a = its[i].get();
            Iterator* b;
            if (i + 1 < its.size()) {
                b = its[i + 1].get();
            } else {
                b = &eofIterator;
            }
            bits_[last++].init(a, b);
        }
        end = last;

        while (start + 1 != end) {
            for (size_t i = start; i < end; i += 2) {
                BufferedBinaryMergeIterator* ba = &bits_[i];
                BufferedBinaryMergeIterator* bb;
                if (i + 1 < end) {
                    bb = &bits_[i + 1];
                } else {
                    bb = &bufferedEofIterator;
                }
                bits_[last++].init(ba, bb);
            }
            start = end;
            end = last;
        }

        root_ = &bits_[last - 1];
    }

    bool has() const { return root_->has(); }

    int get() const { return root_->get(); }

    void next() { root_->next(); }

    BufferedBinaryMergeIterator* root_;
    vector<BufferedBinaryMergeIterator> bits_;
};

template <typename T>
int sumIterator(T& it) {
    int result = 0;
    int prev = -1;
    while (it.has()) {
        int now = it.get();
        result += now;
        if (prev > now) {
            LOG(ERROR) << "Unordered " << OUT(prev) << OUT(now);
        }
        prev = now;
        it.next();
    }
    return result;
}

int main() {
    genData();

    {
        Timer tSort("Sort");
        IntVector res;
        {
            Timer tCat("Cat");
            res.reserve(kM * kN);
            for (const auto& v : data) {
                res.insert(res.end(), v.begin(), v.end());
            }
        }
        sort(res);
        LOG(INFO) << OUT(sum(res));
    }

    {
        Timer tSort("RadixSort");
        U32Vector res;
        {
            res.reserve(kM * kN);
            for (const auto& v : data) {
                res.insert(res.end(), v.begin(), v.end());
            }
        }
        radixSort(res);
        LOG(INFO) << OUT(sum(res));
    }

    auto makePIterators = []() {
        PIterators result;
        for (size_t i = 0; i < data.size(); ++i) {
            result.emplace_back(make_shared<Iterator>(i));
        }
        return result;
    };

    {
        Timer tHeapMerge("Heap merge");
        auto pits = makePIterators();
        HeapMergeIterator m(pits);
        LOG(INFO) << OUT(sumIterator(m));
    }

    {
        Timer tTreeMerge("Tree merge");
        auto pits = makePIterators();
        BinaryTreeMergeIterator m(pits);
        LOG(INFO) << OUT(sumIterator(m));
    }

    {
        Timer tBufferedTreeMerge("Buffered tree merge");
        auto pits = makePIterators();
        BufferedBinaryTreeMergeIterator m(pits);
        LOG(INFO) << OUT(sumIterator(m));
    }

    {
        Timer tBufferedTreeMerge("Buffered tree merge 2");
        auto pits = makePIterators();
        BufferedBinaryTreeMergeIterator2 m(pits);
        LOG(INFO) << OUT(sumIterator(m));
    }

    return 0;
}
