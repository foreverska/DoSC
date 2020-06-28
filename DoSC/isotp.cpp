#include "isotp.h"

tSessionMap::iterator IsoTp::MakeSession(uint32_t id)
{
    tSessionMap::iterator it;

    it = m_sessionMap.emplace(std::make_pair(id, id)).first;

    it->second.SetCan(m_pCan);

    tSessionTimeoutCb timeout;
    timeout = std::bind(&IsoTp::SessionTimeout, this, std::placeholders::_1);
    it->second.SetTimeoutCb(timeout);

    tSessionFinishedCb finished;
    finished = std::bind(&IsoTp::SessionFinished, this, std::placeholders::_1);
    it->second.SetFinishedCb(finished);

    return it;
}

void IsoTp::IngestFrame(tCanFrame &frame)
{
    std::unique_lock<std::mutex> sessionLock(m_sessionMutex);

    tSessionMap::iterator it;
    it = m_sessionMap.find(frame.id);

    if (it != m_sessionMap.end())
    {
        if (it->second.IsTimedOut())
        {
            m_sessionMap.erase(it);
            it = MakeSession(frame.id);
        }
        it->second.AddSessionData(frame);
    }
    else
    {
        it = MakeSession(frame.id);
        it->second.AddSessionData(frame);
    }
}

void IsoTp::SessionTimeout(uint32_t id)
{
    std::unique_lock<std::mutex> sessionLock(m_sessionMutex);
    printf("IsoTpSesssion with %x timed out\n", id);
}

void IsoTp::SessionFinished(tTpMsg msg)
{
    printf("IsoTpSession with %x finished with %ld bytes of data\n",
           msg.id,
           msg.data.size());

    if (m_finishedCb != nullptr)
    {
        m_finishedCb(msg);
    }
}

void IsoTp::SetCan(tSharedCan pCan)
{
    m_pCan = pCan;
}

void IsoTp::SetFinishedCb(tFinishedCb &cb)
{
    m_finishedCb = cb;
}

IsoTp::IsoTp()
{
    m_finishedCb = nullptr;
}
