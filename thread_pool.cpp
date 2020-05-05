#include "thread_pool.h"

#include <utility>

ThreadPool::ThreadPool(std::size_t threads_num)
{
    for (std::size_t i = 0; i < threads_num; ++i) {
        workers_.emplace_back(
                [this](){
                    while (true) {
                        std::unique_lock<std::mutex> lk(this->cv_m_);
                        condition_.wait(lk, [this](){
                            return !this->tasks_.empty() || this->quit_;
                        });
                        if (this->quit_ && this->tasks_.empty()) {
                            return;
                        }
                        if (!this->tasks_.empty()) {
                            auto f = std::move(this->tasks_.front());
                            this->tasks_.pop();
                            lk.unlock();
                            f.get();
                        }
                    }
                }
        );
    }
}

ThreadPool::~ThreadPool() {
    quit_ = true;
    join();
}

void ThreadPool::join() {
    condition_.notify_all();
    for (auto& worker : workers_) {
        worker.join();
    }
}

