#ifndef SRC_PARALLEL_HPP
#define SRC_PARALLEL_HPP

#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <condition_variable>
#include <barrier>

namespace ras
{
    class ThreadPool
    {
      private:
        std::atomic_bool running = true;
        std::barrier<> begin_;
        std::barrier<> end_;

        std::vector<std::function<void()>> calls_ = {};
        std::vector<std::thread> ths_ = {};

      public:
        ThreadPool();
        ~ThreadPool();

        bool add_task(const std::function<void()>& call);
        void wait();
        void begin();
    };

    // one read one out
    template <typename T>
    class RingQueue
    {
      private:
        size_t size_ = 0;
        std::atomic_ref<size_t> atomic_size_;
        std::vector<T> arr_ = {};
        size_t front_ = 0;
        size_t back_ = 0;

      public:
        RingQueue(size_t max_size);

        bool pop(T& target);
        bool push(const T& elem);
        size_t capacity();
        size_t size() { return size_; }
    };

    class AdvanceThreadPool
    {
      private:
        std::atomic_bool running_ = true;
        std::atomic_bool manager_free_ = true;

        std::mutex ready_;
        std::thread manager_;
        std::vector<std::thread> workers_;
        std::barrier<> worker_begin_;
        std::barrier<> worker_end_;

        const std::vector<std::function<void()>>* all_tasks_ = nullptr;
        std::vector<RingQueue<const std::function<void()>*>> jobs_;

      public:
        std::atomic_bool enable_sleeping_ = true;

        AdvanceThreadPool(size_t jobs_size);
        ~AdvanceThreadPool();

        void add_tasks(const std::vector<std::function<void()>>& funcs);
        bool is_manager_empty();
        void wait_all_workers();
        void prepare_and_start();
    };

}; // namespace ras

template <typename T>
inline ras::RingQueue<T>::RingQueue(size_t max_size)
    : arr_(max_size),
      atomic_size_(size_)
{
}

template <typename T>
inline bool ras::RingQueue<T>::pop(T& target)
{
    if (atomic_size_ == 0)
    {
        return false;
    }

    target = arr_[front_];
    front_++;
    front_ %= capacity();
    atomic_size_--;
    return true;
}

template <typename T>
inline bool ras::RingQueue<T>::push(const T& elem)
{
    if (atomic_size_ == capacity())
    {
        return false;
    }

    arr_[back_] = elem;
    back_++;
    back_ %= capacity();
    atomic_size_++;
    return true;
}

template <typename T>
inline size_t ras::RingQueue<T>::capacity()
{
    return arr_.size();
}

#endif // SRC_PARALLEL_HPP
