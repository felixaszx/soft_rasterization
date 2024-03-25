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
        : manager_(
              [&]()
              {
                  while (true)
                  {
                      ready_.lock();
                      if (!running_)
                      {
                          break;
                      }

                      if (all_tasks_)
                      {
                         
                      }

                      ready_.unlock();
                  }
              })
    {
        ready_.lock();
    }

    AdvanceThreadPool::~AdvanceThreadPool()
    {
        wait_all();
        running_ = false;
        prepare();
        manager_.join();
        for (auto& worker : workers_)
        {
            worker.join();
        }
    }

    void AdvanceThreadPool::wait_all()
    {
        ready_.lock();
    }

    bool AdvanceThreadPool::is_empty()
    {
        return ready_.try_lock();
    }

    void AdvanceThreadPool::prepare()
    {
        ready_.unlock();
    }
}; // namespace ras