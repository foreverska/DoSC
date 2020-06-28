#ifndef ISOTPSESSION_H
#define ISOTPSESSION_H

#include <stdint.h>

#include <vector>

#include "resettabletimer.h"
#include "can.h"

typedef enum
{
    UNINIT,
    INPROGRESS,
    DONE
} tSessionStatus;

typedef struct
{
     uint32_t id;
     std::vector<uint8_t> data;
} tTpMsg;

typedef std::function<void(uint32_t)> tSessionTimeoutCb;
typedef std::function<void(tTpMsg)> tSessionFinishedCb;

class IsoTpSession
{
private:
    uint32_t m_id;
    uint32_t m_finalSize;
    uint32_t m_currentIndex;
    ResettableTimer m_timer;
    tTpMsg m_msg;
    tSharedCan m_pCan;

    tSessionStatus m_status;

    tSessionTimeoutCb m_timeoutCb;
    tSessionFinishedCb m_finishedCb;

    void SessionTimeout();
    void SessionDone();

    void ProcessSingle(tCanFrame &frame);
    void ProcessFirst(tCanFrame &frame);
    void ProcessConsec(tCanFrame &frame);
    void ProcessFlow(tCanFrame &frame);

    void SendFlow(uint8_t flag, tCanFrame &frame);

protected:

public:
    bool IsTimedOut();
    void AddSessionData(tCanFrame &frame);

    void SetTimeoutCb(tSessionTimeoutCb &cb);
    void SetFinishedCb(tSessionFinishedCb &cb);

    void SetCan(tSharedCan pCan);
    IsoTpSession(uint32_t id);
};

#endif // ISOTPSESSION_H
