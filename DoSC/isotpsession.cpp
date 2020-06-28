#include "isotpsession.h"

#define CODE_SINGLE 0
#define CODE_FIRST  1
#define CODE_CONSEC 2
#define CODE_FLOW   3

#define CODE_BYTE   0
#define FIRST_SIZE  1
#define CONSEC_DATA 1
#define SINGLE_SIZE 0
#define START_INDEX 0x1
#define MAX_INDEX   0xF

#define FLOW_CONT   0
#define FLOW_WAIT   1
#define FLOW_ABRT   2

#define LOW_MIN     2

#define HIGH_NIBBLE 0xF0
#define LOW_NIBBLE 0x0F
#define HIGH_TO_LOW 4
#define BYTE        8

#define EMPTY_SIZE 0

uint32_t ReverseId(uint32_t id)
{
    uint32_t txId;

    if (id > 0x7E0 && id < 0x7E8)
    {
        return id += 0x008;
    }
    else if (id >= 0x7E8 && id < 0x7F0)
    {
        return  id -= 0x008;
    }
    else if ((id & 0x18DA0000) == 0x18DA0000)
    {
        txId = 0x18DA0000;
        txId |= uint32_t(id & 0x0000FF00) >> 8;
        txId |= uint32_t(id & 0x000000FF) << 8;
        return txId;
    }

    return 0x0;
}

void IsoTpSession::SendFlow(uint8_t flag, tCanFrame &frame)
{
    tCanFrame flowFrame;

    flowFrame.id = ReverseId(frame.id);
    if (flowFrame.id == 0x0)
    {
        return;
    }

    flowFrame.size = 8;
    flowFrame.data[0] = 0x30 | flag;

    m_pCan->SendCanFrame(flowFrame);
}

bool IsoTpSession::IsTimedOut()
{
    return m_timer.IsTimedOut();
}

void IsoTpSession::SessionTimeout()
{
    if (m_timeoutCb != nullptr)
    {
        m_timeoutCb(m_id);
    }
}

void IsoTpSession::ProcessFirst(tCanFrame &frame)
{
    if (frame.size < LOW_MIN)
    {
        SendFlow(FLOW_ABRT, frame);
        return;
    }

    m_finalSize = uint32_t((frame.data[CODE_BYTE] & LOW_NIBBLE) << BYTE) +
                  frame.data[FIRST_SIZE];

    m_msg.id = m_id;
    for (size_t i = LOW_MIN; i < frame.size; i++)
    {
        m_msg.data.push_back(frame.data[i]);
    }

    if (m_msg.data.size() >= m_finalSize)
    {
        m_finishedCb(m_msg);
        return;
    }

    m_timer.Start();
    m_status = INPROGRESS;
    m_currentIndex = START_INDEX;

    SendFlow(FLOW_CONT, frame);
}

void IsoTpSession::ProcessConsec(tCanFrame &frame)
{
    if (frame.size < LOW_MIN)
    {
        return;
    }

    uint8_t index = frame.data[CODE_BYTE] & LOW_NIBBLE;
    if (m_currentIndex != index)
    {
        m_status = UNINIT;
        return;
    }

    m_currentIndex++;
    if (m_currentIndex > MAX_INDEX)
    {
        m_currentIndex = START_INDEX;
    }

    for (size_t i = CONSEC_DATA; i < frame.size; i++)
    {
        m_msg.data.push_back(frame.data[i]);

        if (m_msg.data.size() >= m_finalSize)
        {
            m_timer.Stop();
            m_finishedCb(m_msg);
            m_status = DONE;
            return;
        }
    }

    m_timer.Reset();
}

void IsoTpSession::ProcessFlow(tCanFrame &frame)
{
    (void)frame;
}

void IsoTpSession::ProcessSingle(tCanFrame &frame)
{
    uint8_t isoSize;
    tTpMsg msg;

    if(frame.size < LOW_MIN)
    {
        return;
    }

    isoSize = frame.data[SINGLE_SIZE];
    if (frame.size < isoSize + 1)
    {
        return;
    }

    for (size_t i = 0; i < isoSize; i++)
    {
        msg.data.push_back(frame.data[i+1]);
    }
    msg.id = m_id;

    if (m_finishedCb != nullptr)
    {
        m_finishedCb(msg);
    }
}

void IsoTpSession::AddSessionData(tCanFrame &frame)
{
    if (frame.id != m_id)
    {
        return;
    }
    if (frame.size == EMPTY_SIZE)
    {
        return;
    }

    uint8_t code = ((frame.data[CODE_BYTE] & HIGH_NIBBLE) >> HIGH_TO_LOW);
    if (code == CODE_SINGLE)
    {
        ProcessSingle(frame);
    }
    if (code == CODE_FIRST)
    {
        if (m_status == INPROGRESS)
        {
            return;
        }

        ProcessFirst(frame);
    }
    if (code == CODE_CONSEC)
    {
        if (m_status != INPROGRESS)
        {
            return;
        }

        ProcessConsec(frame);
    }
    if (code == CODE_FLOW)
    {
        ProcessFlow(frame);
    }
}

void IsoTpSession::SetTimeoutCb(tSessionTimeoutCb &cb)
{
    m_timeoutCb = cb;
}

void IsoTpSession::SetFinishedCb(tSessionFinishedCb &cb)
{
    m_finishedCb = cb;
}

void IsoTpSession::SetCan(tSharedCan pCan)
{
    m_pCan = pCan;
}

IsoTpSession::IsoTpSession(uint32_t id)
{
    m_id = id;
    m_finalSize = EMPTY_SIZE;
    m_status = UNINIT;

    m_timeoutCb = nullptr;
    m_finishedCb = nullptr;

    m_timer.SetCallback(std::bind(&IsoTpSession::SessionTimeout, this));
}
