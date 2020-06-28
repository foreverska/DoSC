#ifndef DFR_H
#define DFR_H

#include <QObject>

#include "dosc.h"

class DFR : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList curDtcs READ curDtcs
               WRITE setCurDtcs NOTIFY curDtcsChanged)

    QStringList m_curDtcs;
    DoSC m_Dosc;

public:
    explicit DFR(QObject *parent = nullptr);

    Q_INVOKABLE void canConnect(QString interface);
    Q_INVOKABLE void getDTCs();
    Q_INVOKABLE void clearDTCs();

    QStringList curDtcs() const;

public slots:
    void setCurDtcs(QStringList curDtcs);

signals:
    void curDtcsChanged(QStringList curDtcs);

};

#endif // DFR_H
