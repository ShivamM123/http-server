#include "thread_pool.h"
#include <iostream>
#include <stdexcept>

using namespace std;

ThreadPool::ThreadPool(size_t numberOfThreads) {
    shutdownFlag = false;
    create_threads(numberOfThreads);
}

ThreadPool::~ThreadPool() {
    cout << "Closing the thread pool...\n";
    if (!shutdownFlag.load()) {
        shutdown();
    }
    workerThreads.clear();
}

void ThreadPool::create_threads(size_t numberOfThreads) {
    auto workerFn = [this] { worker(); };
    
    for (size_t i = 0; i < numberOfThreads; ++i) {
        try {
            workerThreads.push_back(thread(workerFn));
        }
        catch (const exception& e) {
            cerr << "Error in thread creation: " << e.what() << '\n';
            throw runtime_error("Failed to create threads");
        }
    }
}

void ThreadPool::worker() {
    while (true) {
        function<void()> task;
        {
            unique_lock<mutex> lock(qMutex);
            cv.wait(lock, [this] { return shutdownFlag.load() || !taskQueue.empty(); });

            if (shutdownFlag.load() && taskQueue.empty()) {
                break;
            }

            task = move(taskQueue.front());
            taskQueue.pop();
        } 

        try {
            task();
        }
        catch (const exception& e) {
            cerr << "Task threw an exception: " << e.what() << '\n';
        }
        catch (...) {
            cerr << "Task threw an unknown error\n";
        }
    }
}

void ThreadPool::submit(function<void()> task) {
    if (shutdownFlag.load()) {
        throw runtime_error("Cannot submit tasks after calling shutdown");
    }
    
    {
        unique_lock<mutex> lock(qMutex);
        taskQueue.push(move(task));
    }
    cv.notify_one();
}

void ThreadPool::shutdown() {
    {
        unique_lock<mutex> lock(qMutex);
        shutdownFlag = true;
    }
    
    cv.notify_all(); 

    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            try {
                thread.join(); 
            } 
            catch (const exception& e) {
                cerr << "Error in joining the thread: " << e.what() << '\n';
            }
        }
    }
}