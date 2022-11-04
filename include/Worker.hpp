#pragma once
#include <iostream>
#include <atomic>
#include <functional>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>

using Work = std::function<void(std::size_t)>;

struct Worker
{

    Worker()
    {
        static std::atomic<size_t> counter(0);
        _id = counter++;
        is_stop.store(false);
        worker_thread = std::thread(&Worker::dispatch_loop, this);
    }
    ~Worker()
    {
        stop();
        work_queue_notifier.notify_one();
        worker_thread.join();
    }

    void dispatch(Work &&w)
    {
        work_queue.emplace(w);
        work_queue_notifier.notify_one();
    }

    template <typename Func, typename... Args>
    void dispatch(Func &&work, Args &&...args)
    {
        work_queue.emplace(std::bind(std::forward<Func>(work), std::forward<Args>(args)...));
        work_queue_notifier.notify_one();
    }

    std::size_t get_id() const { return _id; }
    void stop() { is_stop.store(true); }

private:
    void dispatch_loop()
    {
        while (!is_stop)
        {
            Work w = nullptr;
            {
                std::unique_lock<std::mutex> lock(work_queue_mutex);
                work_queue_notifier.wait(lock, [this]()
                                         { return !work_queue.empty() || is_stop; });
                if (!is_stop)
                {

                    w = work_queue.front();
                    work_queue.pop();
                }
            }
            if (!is_stop && w != nullptr)
            {
                w(_id);
            }
        }
    }
    std::queue<Work> work_queue;
    std::size_t _id;
    std::atomic<bool> is_stop;
    std::mutex work_queue_mutex;
    std::condition_variable work_queue_notifier;
    std::thread worker_thread;
};
