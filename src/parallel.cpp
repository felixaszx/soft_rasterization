#include "parallel.hpp"

#include <iostream>

namespace ras
{
    ThreadPool::ThreadPool()
        : begin_(std::thread::hardware_concurrency() + 1),
          end_(std::thread::hardware_concurrency() + 1)
    {
        srand(time(nullptr));
        for (int i = 0; i < std::thread::hardware_concurrency(); i++)
        {
            calls_.emplace_back([]() {});
            ths_.emplace_back(
                [&, i]()
                {
                    while (true)
                    {
                        begin_.arrive_and_wait();
                        if (!running)
                        {
                            break;
                        }
                        calls_[i]();
                        end_.arrive_and_wait();
                    }
                });
        }
    }

    ThreadPool::~ThreadPool()
    {
        running = false;
        begin();
        for (auto& th : ths_)
        {
            th.join();
        }
    }

    bool ThreadPool::add_task(const std::function<void()>& call)
    {
        if (calls_.size() < ths_.size())
        {
            calls_.push_back(call);
            return true;
        }
        return false;
    }

    void ThreadPool::wait()
    {
        end_.arrive_and_wait();
        calls_.clear();
    }

    void ThreadPool::begin()
    {
        begin_.arrive_and_wait();
    }

    AdvanceThreadPool::AdvanceThreadPool(size_t jobs_size)
        : worker_begin_(std::thread::hardware_concurrency() + 1),
          worker_end_(std::thread::hardware_concurrency() + 2),
          manager_(
              [&]()
              {
                  while (true)
                  {
                      ready_.lock();
                      manager_free_ = false;
                      worker_begin_.arrive_and_wait();

                      if (!running_)
                      {
                          ready_.unlock();
                          return;
                      }

                      if (all_tasks_)
                      {
                          size_t w = 0;
                          size_t f = 0;
                          while (w++, w %= jobs_.size(), f < all_tasks_->size())
                          {
                              if (jobs_[w].push(all_tasks_->data() + f))
                              {
                                  f++;
                              }
                          }
                      }

                      all_tasks_ = nullptr;
                      manager_free_ = true;
                      worker_end_.arrive_and_wait();
                  }
              })
    {
        workers_.reserve(std::thread::hardware_concurrency());
        jobs_.reserve(std::thread::hardware_concurrency());

        for (int i = 0; i < std::thread::hardware_concurrency(); i++)
        {
            jobs_.emplace_back(jobs_size);
            workers_.emplace_back(
                [&, i]()
                {
                    while (true)
                    {
                        worker_begin_.arrive_and_wait();

                        if (!running_)
                        {
                            return;
                        }

                        bool test = false;
                        const std::function<void()>* func = nullptr;
                        while (test = jobs_[i].pop(func), !manager_free_ || test)
                        {
                            if (test)
                            {
                                (*func)();
                            }
                        }

                        worker_end_.arrive_and_wait();
                    }
                });
        }
    }

    AdvanceThreadPool::~AdvanceThreadPool()
    {
        running_ = false;
        prepare_and_start();
        wait_all_workers();
        for (auto& worker : workers_)
        {
            worker.join();
        }
        manager_.join();
    }

    void AdvanceThreadPool::add_tasks(const std::vector<std::function<void()>>& funcs)
    {
        all_tasks_ = &funcs;
    }

    void AdvanceThreadPool::wait_all_workers()
    {
        worker_end_.arrive_and_wait();
    }

    bool AdvanceThreadPool::is_manager_empty()
    {
        return manager_free_;
    }

    void AdvanceThreadPool::prepare_and_start()
    {
        ready_.unlock();
    }
}; // namespace ras