#ifndef SRC_PARALLEL_HPP
#define SRC_PARALLEL_HPP

#include <vector>
#include <atomic>
#include <functional>
#include <thread>
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
        std::atomic_size_t size_ = 0;
        std::vector<T> arr_ = {};
        size_t front_ = 0;
        size_t back_ = 0;

      public:
        RingQueue(size_t max_size);

        bool pop(T& target);
        bool push(const T& elem);
        size_t capacity();
    };

    class AdvanceThreadPool
    {
      private:
        std::atomic_bool running_ = true;
        std::mutex ready_;
        std::thread manager_;
        std::vector<std::thread> workers_;

        size_t frequency_ = 10;
        const std::vector<std::function<void()>>* all_tasks_ = nullptr;
        std::vector<RingQueue<std::function<void()>>> jobs_;

      public:
        AdvanceThreadPool(size_t jobs_size);
        ~AdvanceThreadPool();

        void add_tasks(const std::vector<std::function<void()>>& funcs);
        void wait_all();
        bool is_empty();
        void prepare();
    };

}; // namespace ras

template <typename T>
inline ras::RingQueue<T>::RingQueue(size_t max_size)
    : arr_(max_size)
{
}

template <typename T>
inline bool ras::RingQueue<T>::pop(T& target)
{
    if (size_ == 0)
    {
        return false;
    }

    target = arr_[front_];
    front_++;
    front_ %= capacity();
    size_--;
    return true;
}

template <typename T>
inline bool ras::RingQueue<T>::push(const T& elem)
{
    if (size_ >= capacity())
    {
        return false;
    }

    arr_[back_] = elem;
    back_++;
    back_ %= capacity();
    size_++;
    return true;
}

template <typename T>
inline size_t ras::RingQueue<T>::capacity()
{
    return arr_.size();
}

#endif // SRC_PARALLEL_HPP
