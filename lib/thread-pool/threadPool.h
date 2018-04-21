#pragma once

// Copyright (c) 2010-2011 Dmitry Vyukov. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided
// that the following conditions are met:
//
//   1. Redistributions of source code must retain the above copyright notice,
//   this list of
//      conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//   notice, this list
//      of conditions and the following disclaimer in the documentation and/or
//      other materials
//      provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY DMITRY VYUKOV "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT
// SHALL DMITRY VYUKOV OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are
// those of the authors and
// should not be interpreted as representing official policies, either expressed
// or implied, of Dmitry Vyukov.

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <functional>
#include <memory>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace tp {
/**
 * @brief The Worker class owns task queue and executing thread.
 * In thread it tries to pop task from queue. If queue is empty then it tries
 * to steal task from the sibling worker. If steal was unsuccessful then spins
 * with one millisecond delay.
 */
template <typename Task, template <typename> class Queue>
class Worker {
   public:
    /**
     * @brief Worker Constructor.
     * @param queue_size Length of undelaying task queue.
     */
    explicit Worker(size_t queue_size);

    /**
     * @brief Move ctor implementation.
     */
    Worker(Worker&& rhs) noexcept;

    /**
     * @brief Move assignment implementaion.
     */
    Worker& operator=(Worker&& rhs) noexcept;

    /**
     * @brief start Create the executing thread and start tasks execution.
     * @param id Worker ID.
     * @param steal_donor Sibling worker to steal task from it.
     */
    void start(size_t id, Worker* steal_donor);

    /**
     * @brief stop Stop all worker's thread and stealing activity.
     * Waits until the executing thread became finished.
     */
    void stop();

    /**
     * @brief post Post task to queue.
     * @param handler Handler to be executed in executing thread.
     * @return true on success.
     */
    template <typename Handler>
    bool post(Handler&& handler);

    /**
     * @brief steal Steal one task from this worker queue.
     * @param task Place for stealed task to be stored.
     * @return true on success.
     */
    bool steal(Task& task);

    /**
     * @brief getWorkerIdForCurrentThread Return worker ID associated with
     * current thread if exists.
     * @return Worker ID.
     */
    static size_t getWorkerIdForCurrentThread();

   private:
    /**
     * @brief threadFunc Executing thread function.
     * @param id Worker ID to be associated with this thread.
     * @param steal_donor Sibling worker to steal task from it.
     */
    void threadFunc(size_t id, Worker* steal_donor);

    Queue<Task> queue_;
    std::atomic<bool> running_flag_;
    std::thread thread_;
};

/**
 * @brief The ThreadPoolOptions class provides creation options for
 * ThreadPool.
 */
class ThreadPoolOptions {
   public:
    /**
     * @brief ThreadPoolOptions Construct default options for thread pool.
     */
    ThreadPoolOptions();

    /**
     * @brief setThreadCount Set thread count.
     * @param count Number of threads to be created.
     */
    void setThreadCount(size_t count);

    /**
     * @brief setQueueSize Set single worker queue size.
     * @param count Maximum length of queue of single worker.
     */
    void setQueueSize(size_t size);

    /**
     * @brief threadCount Return thread count.
     */
    size_t threadCount() const;

    /**
     * @brief queueSize Return single worker queue size.
     */
    size_t queueSize() const;

   private:
    size_t thread_count_;
    size_t queue_size_;
};

/**
 * @brief The MPMCBoundedQueue class implements bounded
 * multi-producers/multi-consumers lock-free queue.
 * Doesn't accept non-movable types as T.
 * Inspired by Dmitry Vyukov's mpmc queue.
 * http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 */
template <typename T>
class MPMCBoundedQueue {
    static_assert(std::is_move_constructible<T>::value, "Should be of movable type");

   public:
    /**
     * @brief MPMCBoundedQueue Constructor.
     * @param size Power of 2 number - queue length.
     * @throws std::invalid_argument if size is bad.
     */
    explicit MPMCBoundedQueue(size_t size);

    /**
     * @brief Move ctor implementation.
     */
    MPMCBoundedQueue(MPMCBoundedQueue&& rhs) noexcept;

    /**
     * @brief Move assignment implementaion.
     */
    MPMCBoundedQueue& operator=(MPMCBoundedQueue&& rhs) noexcept;

    /**
     * @brief push Push data to queue.
     * @param data Data to be pushed.
     * @return true on success.
     */
    template <typename U>
    bool push(U&& data);

    /**
     * @brief push Push data to queue.
     * @param data Data to be pushed.
     */
    template <typename U>
    void blockingPush(U&& data);

    /**
     * @brief pop Pop data from queue.
     * @param data Place to store popped data.
     * @return true on sucess.
     */
    bool pop(T& data);

    std::condition_variable& event() {
        return event_;
    }

    std::unique_lock<std::mutex>& lock() {
        return lk_;
    }

   private:
    struct Cell {
        std::atomic<size_t> sequence;
        T data;

        Cell() = default;

        Cell(const Cell&) = delete;
        Cell& operator=(const Cell&) = delete;

        Cell(Cell&& rhs) : sequence(rhs.sequence.load()), data(std::move(rhs.data)) {}

        Cell& operator=(Cell&& rhs) {
            sequence = rhs.sequence.load();
            data = std::move(rhs.data);

            return *this;
        }
    };

   private:
    using Cacheline = std::array<char, 64>;

    Cacheline pad0_;
    std::vector<Cell> buffer_;
    /* const */ size_t buffer_mask_;
    Cacheline pad1_;
    std::atomic<size_t> enqueue_pos_;
    Cacheline pad2_;
    std::atomic<size_t> dequeue_pos_;
    Cacheline pad3_;
    std::mutex mutex_;
    std::condition_variable event_;
    std::unique_lock<std::mutex> lk_;
};

/**
 * @brief The FixedFunction<R(ARGS...), STORAGE_SIZE> class implements
 * functional object.
 * This function is analog of 'std::function' with limited capabilities:
 *  - It supports only move semantics.
 *  - The size of functional objects is limited to storage size.
 * Due to limitations above it is much faster on creation and copying than
 * std::function.
 */
template <typename SIGNATURE, size_t STORAGE_SIZE = 128>
class FixedFunction;

template <typename R, typename... ARGS, size_t STORAGE_SIZE>
class FixedFunction<R(ARGS...), STORAGE_SIZE> {
    using func_ptr_type = std::function<R(ARGS...)>;

   public:
    FixedFunction() : function_ptr_(nullptr), method_ptr_(nullptr), alloc_ptr_(nullptr) {}

    /**
     * @brief FixedFunction Constructor from functional object.
     * @param object Functor object will be stored in the internal storage
     * using move constructor. Unmovable objects are prohibited explicitly.
     */
    template <typename FUNC>
    FixedFunction(FUNC&& object) : FixedFunction() {
        using unref_type = typename std::remove_reference<FUNC>::type;

        static_assert(sizeof(unref_type) < STORAGE_SIZE, "functional object doesn't fit into internal storage");
        static_assert(std::is_move_constructible<unref_type>::value, "Should be of movable type");

        method_ptr_ = [](void* object_ptr, func_ptr_type, ARGS... args) -> R {
            return static_cast<unref_type*>(object_ptr)->operator()(args...);
        };

        alloc_ptr_ = [](void* storage_ptr, void* object_ptr) {
            if (object_ptr) {
                unref_type* x_object = static_cast<unref_type*>(object_ptr);
                new (storage_ptr) unref_type(std::move(*x_object));
            } else {
                static_cast<unref_type*>(storage_ptr)->~unref_type();
            }
        };

        alloc_ptr_(&storage_, &object);
    }

    /**
     * @brief FixedFunction Constructor from free function or static member.
     */
    template <typename RET, typename... PARAMS>
    FixedFunction(RET (*func_ptr)(PARAMS...)) : FixedFunction() {
        function_ptr_ = func_ptr;
        method_ptr_ = [](void*, func_ptr_type f_ptr, ARGS... args) -> R {
            return static_cast<RET (*)(PARAMS...)>(f_ptr)(args...);
        };
    }

    FixedFunction(FixedFunction&& o) : FixedFunction() { moveFromOther(o); }

    FixedFunction& operator=(FixedFunction&& o) {
        moveFromOther(o);
        return *this;
    }

    ~FixedFunction() {
        if (alloc_ptr_) {
            alloc_ptr_(&storage_, nullptr);
        }
    }

    /**
     * @brief operator () Execute stored functional object.
     * @throws std::runtime_error if no functional object is stored.
     */
    R operator()(ARGS... args) {
        if (!method_ptr_) {
            throw std::runtime_error("call of empty functor");
        }
        return method_ptr_(&storage_, function_ptr_, args...);
    }

   private:
    FixedFunction& operator=(const FixedFunction&) = delete;
    FixedFunction(const FixedFunction&) = delete;

    union {
        typename std::aligned_storage<STORAGE_SIZE, sizeof(size_t)>::type storage_;
        func_ptr_type function_ptr_;
    };

    using method_type = std::function<R(void* object_ptr, func_ptr_type free_func_ptr, ARGS... args)>;
    method_type method_ptr_;

    using alloc_type = std::function<void(void* storage_ptr, void* object_ptr)>;
    alloc_type alloc_ptr_;

    void moveFromOther(FixedFunction& o) {
        if (this == &o) {
            return;
        }

        if (alloc_ptr_) {
            alloc_ptr_(&storage_, nullptr);
            alloc_ptr_ = nullptr;
        } else {
            function_ptr_ = nullptr;
        }

        method_ptr_ = o.method_ptr_;
        o.method_ptr_ = nullptr;

        if (o.alloc_ptr_) {
            alloc_ptr_ = o.alloc_ptr_;
            alloc_ptr_(&storage_, &o.storage_);
        } else {
            function_ptr_ = o.function_ptr_;
        }
    }
};

template <typename Task, template <typename> class Queue>
class ThreadPoolImpl;
using ThreadPool = ThreadPoolImpl<FixedFunction<void(), 128>, MPMCBoundedQueue>;

/**
 * @brief The ThreadPool class implements thread pool pattern.
 * It is highly scalable and fast.
 * It is header only.
 * It implements both work-stealing and work-distribution balancing
 * startegies.
 * It implements cooperative scheduling strategy for tasks.
 */
template <typename Task, template <typename> class Queue>
class ThreadPoolImpl {
   public:
    /**
     * @brief ThreadPool Construct and start new thread pool.
     * @param options Creation options.
     */
    explicit ThreadPoolImpl(const ThreadPoolOptions& options = ThreadPoolOptions());

    /**
     * @brief Move ctor implementation.
     */
    ThreadPoolImpl(ThreadPoolImpl&& rhs) noexcept;

    /**
     * @brief ~ThreadPool Stop all workers and destroy thread pool.
     */
    ~ThreadPoolImpl();

    /**
     * @brief Move assignment implementaion.
     */
    ThreadPoolImpl& operator=(ThreadPoolImpl&& rhs) noexcept;

    /**
     * @brief post Try post job to thread pool.
     * @param handler Handler to be called from thread pool worker. It has
     * to be callable as 'handler()'.
     * @return 'true' on success, false otherwise.
     * @note All exceptions thrown by handler will be suppressed.
     */
    template <typename Handler>
    bool tryPost(Handler&& handler);

    /**
     * @brief post Post job to thread pool.
     * @param handler Handler to be called from thread pool worker. It has
     * to be callable as 'handler()'.
     * @throw std::overflow_error if worker's queue is full.
     * @note All exceptions thrown by handler will be suppressed.
     */
    template <typename Handler>
    void post(Handler&& handler);

    template <typename Handler>
    void blockingPost(Handler&& handler);

   private:
    Worker<Task, Queue>& getWorker();

    std::vector<std::unique_ptr<Worker<Task, Queue>>> workers_;
    std::atomic<size_t> next_worker_;
};

/// Implementation

namespace detail {
inline size_t* thread_id() {
    static thread_local size_t tss_id = -1u;
    return &tss_id;
}
}

template <typename Task, template <typename> class Queue>
inline Worker<Task, Queue>::Worker(size_t queue_size) : queue_(queue_size), running_flag_(true) {}

template <typename Task, template <typename> class Queue>
inline Worker<Task, Queue>::Worker(Worker&& rhs) noexcept {
    *this = rhs;
}

template <typename Task, template <typename> class Queue>
inline Worker<Task, Queue>& Worker<Task, Queue>::operator=(Worker&& rhs) noexcept {
    if (this != &rhs) {
        queue_ = std::move(rhs.queue_);
        running_flag_ = rhs.running_flag_.load();
        thread_ = std::move(rhs.thread_);
    }
    return *this;
}

template <typename Task, template <typename> class Queue>
inline void Worker<Task, Queue>::stop() {
    running_flag_.store(false, std::memory_order_relaxed);
    queue_.event().notify_one();
    thread_.join();
}

template <typename Task, template <typename> class Queue>
inline void Worker<Task, Queue>::start(size_t id, Worker* steal_donor) {
    thread_ = std::thread(&Worker<Task, Queue>::threadFunc, this, id, steal_donor);
}

template <typename Task, template <typename> class Queue>
inline size_t Worker<Task, Queue>::getWorkerIdForCurrentThread() {
    return *detail::thread_id();
}

template <typename Task, template <typename> class Queue>
template <typename Handler>
inline bool Worker<Task, Queue>::post(Handler&& handler) {
    return queue_.push(std::forward<Handler>(handler));
}

template <typename Task, template <typename> class Queue>
inline bool Worker<Task, Queue>::steal(Task& task) {
    return queue_.pop(task);
}

template <typename Task, template <typename> class Queue>
inline void Worker<Task, Queue>::threadFunc(size_t id, Worker* steal_donor) {
    *detail::thread_id() = id;

    Task handler;

    while (running_flag_.load(std::memory_order_relaxed)) {
        if (queue_.pop(handler) || steal_donor->steal(handler)) {
            try {
                handler();
            } catch (...) {
                // suppress all exceptions
            }
        } else {
            queue_.event().wait_for(queue_.lock(), std::chrono::milliseconds(1));
        }
    }
}

template <typename Task, template <typename> class Queue>
inline ThreadPoolImpl<Task, Queue>::ThreadPoolImpl(const ThreadPoolOptions& options)
    : workers_(options.threadCount()), next_worker_(0) {
    for (auto& worker_ptr : workers_) {
        worker_ptr = std::make_unique<Worker<Task, Queue>>(options.queueSize());
    }

    for (size_t i = 0; i < workers_.size(); ++i) {
        auto steal_donor = workers_[(i + 1) % workers_.size()].get();
        workers_[i]->start(i, steal_donor);
    }
}

template <typename Task, template <typename> class Queue>
inline ThreadPoolImpl<Task, Queue>::ThreadPoolImpl(ThreadPoolImpl<Task, Queue>&& rhs) noexcept {
    *this = rhs;
}

template <typename Task, template <typename> class Queue>
inline ThreadPoolImpl<Task, Queue>::~ThreadPoolImpl() {
    for (auto& worker_ptr : workers_) {
        worker_ptr->stop();
    }
}

template <typename Task, template <typename> class Queue>
inline ThreadPoolImpl<Task, Queue>& ThreadPoolImpl<Task, Queue>::operator=(ThreadPoolImpl<Task, Queue>&& rhs) noexcept {
    if (this != &rhs) {
        workers_ = std::move(rhs.workers_);
        next_worker_ = rhs.next_worker_.load();
    }
    return *this;
}

template <typename Task, template <typename> class Queue>
template <typename Handler>
inline bool ThreadPoolImpl<Task, Queue>::tryPost(Handler&& handler) {
    return getWorker().post(std::forward<Handler>(handler));
}

template <typename Task, template <typename> class Queue>
template <typename Handler>
inline void ThreadPoolImpl<Task, Queue>::post(Handler&& handler) {
    const auto ok = tryPost(std::forward<Handler>(handler));
    if (!ok) {
        throw std::runtime_error("thread pool queue is full");
    }
}

template <typename Task, template <typename> class Queue>
template <typename Handler>
inline void ThreadPoolImpl<Task, Queue>::blockingPost(Handler&& handler) {
    while (!tryPost(handler)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

template <typename Task, template <typename> class Queue>
inline Worker<Task, Queue>& ThreadPoolImpl<Task, Queue>::getWorker() {
    auto id = Worker<Task, Queue>::getWorkerIdForCurrentThread();

    if (id > workers_.size()) {
        id = next_worker_.fetch_add(1, std::memory_order_relaxed) % workers_.size();
    }

    return *workers_[id];
}

template <typename T>
inline MPMCBoundedQueue<T>::MPMCBoundedQueue(size_t size)
    : buffer_(size), buffer_mask_(size - 1), enqueue_pos_(0), dequeue_pos_(0), lk_(mutex_) {
    bool size_is_power_of_2 = (size >= 2) && ((size & (size - 1)) == 0);
    if (!size_is_power_of_2) {
        throw std::invalid_argument("buffer size should be a power of 2");
    }

    for (size_t i = 0; i < size; ++i) {
        buffer_[i].sequence = i;
    }
}

template <typename T>
inline MPMCBoundedQueue<T>::MPMCBoundedQueue(MPMCBoundedQueue&& rhs) noexcept {
    *this = rhs;
}

template <typename T>
inline MPMCBoundedQueue<T>& MPMCBoundedQueue<T>::operator=(MPMCBoundedQueue&& rhs) noexcept {
    if (this != &rhs) {
        buffer_ = std::move(rhs.buffer_);
        buffer_mask_ = std::move(rhs.buffer_mask_);
        enqueue_pos_ = rhs.enqueue_pos_.load();
        dequeue_pos_ = rhs.dequeue_pos_.load();
    }
    return *this;
}

template <typename T>
template <typename U>
inline bool MPMCBoundedQueue<T>::push(U&& data) {
    Cell* cell;
    size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
    for (;;) {
        cell = &buffer_[pos & buffer_mask_];
        size_t seq = cell->sequence.load(std::memory_order_acquire);
        intptr_t dif = (intptr_t)seq - (intptr_t)pos;
        if (dif == 0) {
            if (enqueue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                break;
            }
        } else if (dif < 0) {
            return false;
        } else {
            pos = enqueue_pos_.load(std::memory_order_relaxed);
        }
    }

    cell->data = std::forward<U>(data);

    cell->sequence.store(pos + 1, std::memory_order_release);

    event_.notify_one();

    return true;
}

template <typename T>
template <typename U>
inline void MPMCBoundedQueue<T>::blockingPush(U&& data) {
    while (!push(data)) {
    }
}

template <typename T>
inline bool MPMCBoundedQueue<T>::pop(T& data) {
    Cell* cell;
    size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
    for (;;) {
        cell = &buffer_[pos & buffer_mask_];
        size_t seq = cell->sequence.load(std::memory_order_acquire);
        intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);
        if (dif == 0) {
            if (dequeue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                break;
            }
        } else if (dif < 0) {
            return false;
        } else {
            pos = dequeue_pos_.load(std::memory_order_relaxed);
        }
    }

    data = std::move(cell->data);

    cell->sequence.store(pos + buffer_mask_ + 1, std::memory_order_release);

    return true;
}
}
