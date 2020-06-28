#include "dosc.h"

void DoSC::ProcessOb2Name(tResultStringMap *pMap, tTpMsg msg)
{
    std::string name;

    if (msg.data[1] != 0x0A)
    {
        return;
    }

    for (size_t i = 3; i < msg.data.size(); i++)
    {
        if (msg.data[i] == '\0')
        {
            break;
        }

        name += static_cast<char>(msg.data[i]);
    }

    (*pMap)[msg.id].push_back(name);
}

void DoSC::Obd2GetName(uint32_t id, tResultStringMap &map)
{
    tCanFrame frame;

    tProcCall call = std::bind(&DoSC::ProcessOb2Name, this, &map,
                               std::placeholders::_1);
    RegisterCodeFunc(0x49, call);

    frame.id = id;
    frame.data[0] = 0x02;
    frame.data[1] = 0x09;
    frame.data[2] = 0x0A;
    frame.size = 03;
    m_pCan->SendCanFrame(frame);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    RegisterCodeFunc(0x49, nullptr);
}

char DtcFirstChar(uint8_t val)
{
    switch (val)
    {
        case 0:
            return 'P';
        case 1:
            return 'C';
        case 2:
            return 'B';
        case 3:
            return 'U';
    }

    return ' ';
}

char NibbleToDtcChar(uint8_t nibble)
{
    if (nibble < 9)
    {
        return char(nibble + '0');
    }

    return char((nibble-10) + 'A');
}

void DoSC::ProcessObd2Faults(tResultStringMap *pMap, tTpMsg msg)
{
    std::string fault;

    size_t size = msg.data.size();
    for (size_t i = 2; i < size; i += 2)
    {
        fault = DtcFirstChar((msg.data[i] & 0xC0) >> 6);
        fault += ((msg.data[i] & 0x30) >> 4) + '0';
        fault += NibbleToDtcChar(msg.data[i] & 0x0F);
        fault += NibbleToDtcChar((msg.data[i+1] & 0xF0) >> 4);
        fault += NibbleToDtcChar(msg.data[i+1] & 0x0F);

        (*pMap)[msg.id].push_back(fault);
    }
}

void DoSC::Obd2GetFaults(uint32_t id, tResultStringMap &map)
{
    tCanFrame frame;

    tProcCall call = std::bind(&DoSC::ProcessObd2Faults, this, &map,
                               std::placeholders::_1);
    RegisterCodeFunc(0x43, call);

    frame.id = id;
    frame.data[0] = 0x01;
    frame.data[1] = 0x03;
    frame.size = 02;
    m_pCan->SendCanFrame(frame);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    RegisterCodeFunc(0x43, nullptr);
}

void DoSC::Obd2ClearFaults(uint32_t id, tResultStringMap &map)
{
    (void) map;
    tCanFrame frame;

    frame.id = id;
    frame.data[0] = 0x01;
    frame.data[1] = 0x04;
    frame.size = 02;
    m_pCan->SendCanFrame(frame);
}
