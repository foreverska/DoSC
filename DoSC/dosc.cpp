#include <stdio.h>

#include "dosc.h"

void PrintCanFrame(tCanFrame &frame)
{
    printf("%x [%d] ", frame.id, frame.size);
    for (size_t i = 0; i < frame.size; i++)
    {
        printf("%x ", frame.data[i]);
    }
    printf("\n");
}

void DoSC::RegisterCodeFunc(uint8_t code, tProcCall call)
{
    std::unique_lock<std::mutex> codeFuncLock(m_codeFuncMutex);

    tCodeProcMap::iterator it = m_codeFunc.find(code);
    if (call == nullptr)
    {
        if (it != m_codeFunc.end())
        {
            m_codeFunc.erase(it);
        }
    }
    else
    {
        if (it != m_codeFunc.end())
        {
            it->second = call;
        }
        else
        {
            m_codeFunc[code] = call;
        }
    }
}

void DoSC::CallCodeFunc(uint8_t code, tTpMsg &msg)
{
    std::unique_lock<std::mutex> codeFuncLock(m_codeFuncMutex);

    tCodeProcMap::iterator it = m_codeFunc.find(code);
    if (it != m_codeFunc.end())
    {
        it->second(msg);
    }
}

void DoSC::IsoTpFinishedCb(tTpMsg &msg)
{
    CallCodeFunc(msg.data[0], msg);
}

void DoSC::CanRxCb(tCanFrame &frame)
{
    PrintCanFrame(frame);
    m_isoTp.IngestFrame(frame);
}

void DoSC::SendCanFrame(tCanFrame &frame)
{
    m_pCan->SendCanFrame(frame);
}

int DoSC::StartCan(std::string &interface)
{
    if (m_pCan->AssignInterface(interface) != CAN_OK)
    {
        return DOSC_NOK;
    }

    if (m_pCan->StartCan() != CAN_OK)
    {
        return DOSC_NOK;
    }

    return DOSC_OK;
}

void DoSC::StopCan()
{
    m_pCan->StopCan();
    m_pCan->UnassignInterface();
}

DoSC::DoSC()
{
    tCanRxCb callback = std::bind(&DoSC::CanRxCb, this, std::placeholders::_1);
    m_pCan = std::make_shared<can>();
    m_pCan->SetRxCb(callback);

    tFinishedCb isoFinished = std::bind(&DoSC::IsoTpFinishedCb, this,
                                        std::placeholders::_1);
    m_isoTp.SetFinishedCb(isoFinished);

    m_isoTp.SetCan(m_pCan);
}

DoSC::~DoSC()
{
    StopCan();
}
