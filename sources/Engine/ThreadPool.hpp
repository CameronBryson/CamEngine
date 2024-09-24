#pragma once
#include <thread>
#include <functional>
#include <mutex>
class thread_pool{
public:
    explicit thread_pool(size_t num_threads);
    ~thread_pool();
    void enqueue(std::function<void()> function);
private:
    std::vector<std::thread> threads;
    std::vector<std::function<void()>> tasks;
    std::mutex mutex;
    bool stop = false;
};