#ifndef ISOTP_H
#define ISOTP_H

#include <mutex>
#include <map>
#include <cinttypes>
#include <functional>

#include <isotpsession.h>
#include "can.h"

typedef std::map<uint32_t, IsoTpSession> tSessionMap;
typedef std::function<void(tTpMsg &)> tFinishedCb;

class IsoTp
{
private:
    std::mutex m_sessionMutex;
    tSessionMap m_sessionMap;
    tSharedCan m_pCan;
    tFinishedCb m_finishedCb;

protected:

    tSessionMap::iterator MakeSession(uint32_t id);
    void SessionTimeout(uint32_t id);
public:
    void IngestFrame(tCanFrame &frame);

    void SessionFinished(tTpMsg msg);

    void SetCan(tSharedCan pCan);
    void SetFinishedCb(tFinishedCb &cb);
    IsoTp();
};

#endif // ISOTP_H
