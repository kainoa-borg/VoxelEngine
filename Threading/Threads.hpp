#pragma once
#include <queue>
#include <thread>
typedef std::function<void()> Task;

class Manager {
public:
    std::mutex gThreadMutex;
    std::condition_variable cvQueue;
    std::condition_variable cvFinished;

    std::vector<std::thread> threads;
    std::queue<Task> taskQueue;
    bool finished;

    void workLoop() {
        std::unique_lock latch(gThreadMutex);
        while(!finished) {
            Task t;
            bool working = false;
            if (!taskQueue.empty()) {
                t = taskQueue.front();
                taskQueue.pop();
                cvFinished.notify_all();
                working = true;
            }
            latch.unlock();
            if (working) {
                t();
            }
            latch.lock();
        }
        latch.unlock();
    }

    void addTask(Task t) {
        std::lock_guard guard(gThreadMutex);
        taskQueue.push(t);
    }

    void alertFinished() {
        std::unique_lock latch(gThreadMutex);
        cvFinished.wait(latch, [this]{return taskQueue.empty();});
        finished = true;
    }

    void finishWork() {
        alertFinished();
        for (auto &worker: threads) {
            worker.join();
        }
    }

    Manager(int n) {
        finished = false;
        for (int i = 0; i < n; i++) {
            std::thread t([this]{workLoop();});
            threads.push_back(std::move(t));
        }
    }
    ~Manager() {
        threads.clear();
        cvFinished.notify_all();
        cvQueue.notify_all();
    }
};