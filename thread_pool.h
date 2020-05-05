#pragma once

#include <future>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <vector>
#include <tuple>
#include <functional>
#include <utility>

class ThreadPool {
using Task = std::future<void>;
public:
    explicit ThreadPool(std::size_t threads_num);
    ~ThreadPool();

    template <typename F, typename ... Args>
    void addTask(F f, Args&& ... args) {
        {
            std::lock_guard<std::mutex> l(cv_m_);
            if (quit_) {
                throw std::runtime_error("adding task to stopped threadpool");
            }
            tasks_.emplace(std::async(std::launch::deferred, std::forward<F>(f),
                    std::forward<Args>(args)...));
        }
        condition_.notify_one();
    }

    void join();

private:
    std::mutex cv_m_;
    std::condition_variable condition_;
    std::atomic_bool quit_{false};
    std::queue<Task> tasks_;
    std::vector<std::thread> workers_;
};

template <typename F, typename ... Args>
void post(ThreadPool& pool, F f, Args&& ... args) {
    pool.addTask(std::forward<F>(f), std::forward<Args>(args)...);
}