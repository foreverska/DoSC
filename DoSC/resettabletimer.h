#ifndef RESETTABLETIMER_H
#define RESETTABLETIMER_H

#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <atomic>

typedef std::unique_lock<std::mutex> tMutexLock;
typedef std::function<void()> tTimerCb;

class ResettableTimer
{
private:
    std::condition_variable m_resetCondition;
    std::mutex m_resetMutex;
    std::chrono::milliseconds m_timeout;

    std::atomic<bool> m_timedOut;
    std::atomic<bool> m_running;
    std::thread m_thread;

    tTimerCb m_timerCallback;

    void RunTimer();

protected:

public:
    void Start();
    void Stop();

    void Reset();

    bool IsTimedOut();

    void SetTimeout(std::chrono::milliseconds timeout);
    void SetCallback(tTimerCb callback);

    ResettableTimer();
    ~ResettableTimer();
};

#endif // RESETTABLETIMER_H
