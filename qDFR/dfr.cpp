#include "dfr.h"

#include <QErrorMessage>

DFR::DFR(QObject *parent) : QObject(parent)
{

}

void DFR::canConnect(QString interface)
{
    std::string newInterface = interface.toStdString();
    m_Dosc.StopCan();
    if (m_Dosc.StartCan(newInterface) == DOSC_NOK)
    {
        qDebug("CAN did not connect correctly");
    }
}

void DFR::getDTCs()
{
    tResultStringMap results;
    m_Dosc.Obd2GetFaults(BROADCAST_11BIT, results);
    QStringList newDtcList;

    if(results.size() == 0)
    {
        newDtcList.push_back("No DTCs");
        setCurDtcs(newDtcList);
        return;
    }

    for (tResultStringMap::iterator it = results.begin();
         it != results.end(); it++)
    {
        for (std::string dtc : it->second)
        {
            newDtcList.push_back(dtc.c_str());
        }
    }

    setCurDtcs(newDtcList);
}

void DFR::clearDTCs()
{
    tResultStringMap results;
    m_Dosc.Obd2ClearFaults(BROADCAST_11BIT, results);
    getDTCs();
}

QStringList DFR::curDtcs() const
{
    return m_curDtcs;
}

void DFR::setCurDtcs(QStringList curDtcs)
{
    if (m_curDtcs == curDtcs)
        return;

    m_curDtcs = curDtcs;
    emit curDtcsChanged(m_curDtcs);
}
