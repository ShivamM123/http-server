#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <utility>

class ThreadPool {
private:
    std::vector<std::thread> workerThreads;
    std::queue<std::function<void()>> taskQueue;
    std::mutex qMutex;
    std::condition_variable cv;
    std::atomic<bool> shutdownFlag;

    void worker();

public:
    ThreadPool(size_t numberOfThreads);
    ~ThreadPool();
    void submit(std::function<void()> task);
    void shutdown();
    void create_threads(size_t numberOfThreads);
};

#endif // THREADPOOL_H