#include "thread_pool.h"
#include <iostream>

using namespace std;

ThreadPool::ThreadPool(size_t numberOfThreads) {
    shutdownFlag = false;
    for(size_t i = 0; i < numberOfThreads; ++i) {
        // Create a thread and move it into our vector
        workerThreads.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::worker() {
    while(true) {
        function<void()> task;
        {
            unique_lock<mutex> lock(qMutex);
            // Wait until there is a task or shutdown is signaled
            cv.wait(lock, [this] { return shutdownFlag.load() || !taskQueue.empty(); });

            if(shutdownFlag.load() && taskQueue.empty()) break;

            task = move(taskQueue.front());
            taskQueue.pop();
        }
        // Execute the task outside the lock
        task();
    }
}

void ThreadPool::submit(function<void()> task) {
    {
        unique_lock<mutex> lock(qMutex);
        taskQueue.push(move(task));
    }
    cv.notify_one(); // Wake up one worker
}

void ThreadPool::shutdown() {
    {
        unique_lock<mutex> lock(qMutex);
        shutdownFlag = true;
    }
    cv.notify_all();
    for(auto& thread : workerThreads) {
        if(thread.joinable()) thread.join();
    }
}