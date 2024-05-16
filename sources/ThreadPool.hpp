//
// Created by cam on 15/05/24.
//

#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t num_workers = std::thread::hardware_concurrency());
    ~ThreadPool();
    template<class F, class ... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
private:
    std::queue<std::function<void()>> task_queue;
    std::condition_variable condition;
    std::mutex queue_mutex;
    std::vector<std::thread> workers;
    bool stop;
};


inline ThreadPool::ThreadPool(std::size_t num_workers) : stop(false){
    for (std::size_t i = 0; i < num_workers; ++i) {
        workers.emplace_back
        (
            [this]
            {
                for(;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{return this->stop || !this->task_queue.empty(); });
                        if(this->stop && this->task_queue.empty())
                            return;
                        task = std::move(this->task_queue.front());
                        this->task_queue.pop();
                    }
                    task();
                }
            }
        );
    }
}

inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}

template<class F, class ... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        task_queue.emplace([task](){(*task)();});
    }
    condition.notify_one();
    return res;
}
#endif //THREADPOOL_HPP
