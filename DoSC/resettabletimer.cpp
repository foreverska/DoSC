#include "resettabletimer.h"

#define DEFAULT_TIMEOUT_MS 1000

bool ResettableTimer::IsTimedOut()
{
    return m_timedOut;
}

void ResettableTimer::RunTimer()
{
    tMutexLock lock(m_resetMutex);

    while (m_running == true)
    {
        if (m_resetCondition.wait_for(lock, m_timeout) ==
            std::cv_status::timeout)
        {
            break;
        }
    }

    if (m_timerCallback != nullptr && m_running == true)
    {
        m_timerCallback();
    }
    m_timedOut = true;
}

void ResettableTimer::Start()
{
    if (m_running == true || m_thread.joinable())
    {
        Stop();
    }

    m_timedOut = false;
    m_running = true;
    m_thread = std::thread(&ResettableTimer::RunTimer, this);
}

void ResettableTimer::Stop()
{
    if (m_thread.joinable() == true)
    {
        m_running = false;
        Reset();
        m_thread.join();
    }
}

void ResettableTimer::Reset()
{
    m_resetCondition.notify_all();
}

void ResettableTimer::SetTimeout(std::chrono::milliseconds timeout)
{
    m_timeout = timeout;
}

void ResettableTimer::SetCallback(tTimerCb callback)
{
    m_timerCallback = callback;
}

ResettableTimer::ResettableTimer()
{
    m_running = false;
    m_timedOut = true;
    m_timeout = std::chrono::milliseconds(DEFAULT_TIMEOUT_MS);
    m_timerCallback = nullptr;
}

ResettableTimer::~ResettableTimer()
{
    Stop();
}
