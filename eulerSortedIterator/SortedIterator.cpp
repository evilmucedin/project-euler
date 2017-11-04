#include <glog/logging.h>

#include "lib/header.h"
#include "lib/random.h"
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

struct Iterator : public IIterator {
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
        return *a < *b;
    }
};

struct HeapMergeIterator : public IIterator {
    HeapMergeIterator(const PIterators& its)
    {
        its_.resize(its.size());
        for (size_t i = 0; i < its.size(); ++i) {
            its_[i] = its[i].get();
        }
        make_heap(its_.begin(), its_.end(), cmp_);
    }

    bool has() const override {
        return !its_.empty();
    }

    int get() const override {
        return its_.front()->get();
    }

    void next() override {
        pop_heap(its_.begin(), its_.end());
        its_.back()->next();
        if (its_.back()->has()) {
            push_heap(its_.begin(), its_.end());
        } else {
            its_.pop_back();
        }
    }

    vector<Iterator*> its_;
    PIteratorsCmp cmp_;
};

struct BinaryMergeIterator : public IIterator {
    BinaryMergeIterator(PIIterator a, PIIterator b) : a_(a), b_(b) {
        calc();
    }

    void calc() {
        bool aHas = a_->has();
        bool bHas = b_->has();
        if (aHas && bHas) {
            has_ = true;
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
            has_ = true;
            val_ = a_->get();
            a_->next();
        } else if (bHas) {
            has_ = true;
            val_ = b_->get();
            b_->next();
        } else {
            has_ = false;
        }
    }

    bool has() const override {
        return has_;
    }

    int get() const override {
        return val_;
    }

    void next() override {
        calc();
    }

    PIIterator a_;
    PIIterator b_;
    int val_;
    bool has_;
};

struct BufferedIterator : public IIterator {
    BufferedIterator(PIIterator it) : it_(it) { fillBuffer(); }

    void fillBuffer() {
        first_ = 0;
        length_ = 0;
        while (length_ < kBuffer && it_->has()) {
            buffer_[length_++] = it_->get();
            it_->next();
        }
    }

    bool has() const {
        return first_ < length_;
    }

    int get() const {
        return buffer_[first_];
    }

    void next() {
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

struct BinaryTreeMergeIterator : public IIterator {
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
                    b = make_shared<Iterator>();
                }
                next.emplace_back(make_shared<BinaryMergeIterator>(a, b));
            }
            now = move(next);
        }
        root_ = now.front();
    }

    bool has() const {
        return root_->has();
    }

    int get() const {
        return root_->get();
    }

    void next() {
        root_->next();
    }

    PIIterator root_;
    PIteratorsCmp cmp_;
};

struct BufferedBinaryTreeMergeIterator : public IIterator {
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

    bool has() const {
        return root_->has();
    }

    int get() const {
        return root_->get();
    }

    void next() {
        root_->next();
    }

    PIIterator root_;
    PIteratorsCmp cmp_;
};

template<typename T>
int sumIterator(T& it) {
    int result = 0;
    while (it.has()) {
        result += it.get();
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
            res.reserve(kM*kN);
            for (const auto& v: data) {
                res.insert(res.end(), v.begin(), v.end());
            }
        }
        sort(res);
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

    return 0;
}
