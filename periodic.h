#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>

class PeriodicRunner
{
public:
    PeriodicRunner() : running(false) {}

    // Starts running the given function periodically at the given microsecond interval
    void start(std::function<void()> func, std::chrono::microseconds interval)
    {
        if (running.load())
        {
            std::cout << "Periodic runner is already running.\n";
            return;
        }

        running.store(true);
        worker = std::thread([this, func, interval]()
                             {
            while (running.load()) {
                auto next_run = std::chrono::steady_clock::now() + interval;
                func();
                std::this_thread::sleep_until(next_run);
            } });
    }

    // Stops the periodic execution
    void stop()
    {
        running.store(false);
        if (worker.joinable())
        {
            worker.join();
        }
    }

    ~PeriodicRunner()
    {
        stop();
    }

private:
    std::atomic<bool> running;
    std::thread worker;
};
