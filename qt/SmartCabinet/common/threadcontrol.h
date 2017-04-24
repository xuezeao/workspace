#ifndef THREADCONTROL_H
#define THREADCONTROL_H

#include <QThread>
#include <QObject>
#include <QDebug>
#include <QTimer>
#include "common/globalvariable.h"
#include "common/timerandupdate.h"
#include "network/netcommunication.h"

class ThreadControl : public QThread
{
    Q_OBJECT
public:
    explicit ThreadControl(QObject *parent = 0);
    void ControlTimerThread(int order);
    void CreateThread();

signals:
    void Close_ALLPage();
    void Update_SQL();

public slots:

private:
    TimerAndUpdate *TC_timerUpdate;
    QThread *timerThread;
    QTimer *timer;

};

#endif // THREADCONTROL_H
