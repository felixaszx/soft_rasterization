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
          manager_(
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
                          manager_free = false;
                          worker_begin_.arrive_and_wait();
                          for (size_t f = 0; f < all_tasks_->size(); f += frequency_)
                          {
                              size_t ff = 0;
                              size_t w = 0;
                              while (f++, ff <= frequency_)
                              {
                                  if (!jobs_[w].push(all_tasks_->at(f + ff - 1)))
                                  {
                                      ff--;
                                      w++;
                                      w %= jobs_.size();
                                  }
                              }

                              w++;
                              w %= jobs_.size();
                          }
                      }

                      all_tasks_ = nullptr;
                      manager_free = true;

                      ready_.unlock();
                  }
              })
    {
        ready_.lock();

        for (int i = 0; i < std::thread::hardware_concurrency(); i++)
        {
            workers_.emplace_back(
                [&, i]()
                {
                    while (true)
                    {
                        worker_begin_.arrive_and_wait();
                        if (!running_)
                        {
                            break;
                        }

                        bool test = false;
                        std::function<void()> func;
                        while (!manager_free || (test = jobs_[i].pop(func), test))
                        {
                            if (test)
                            {
                                std::cout << 1;
                                func();
                            }
                        }
                    }
                });
        }
    }

    AdvanceThreadPool::~AdvanceThreadPool()
    {
        wait_all();
        running_ = false;
        prepare();
        worker_begin_.arrive_and_wait();
        manager_.join();
        for (auto& worker : workers_)
        {
            worker.join();
        }
    }

    void AdvanceThreadPool::add_tasks(const std::vector<std::function<void()>>& funcs, size_t frequency)
    {
        wait_all();
        frequency_ = frequency;
        all_tasks_ = &funcs;
    }

    void AdvanceThreadPool::wait_all()
    {
        ready_.lock();
        std::cout << "K";
    }

    bool AdvanceThreadPool::is_empty()
    {
        return manager_free;
    }

    void AdvanceThreadPool::prepare()
    {
        ready_.unlock();
    }
}; // namespace ras