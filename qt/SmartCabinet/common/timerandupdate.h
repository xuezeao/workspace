#ifndef TIMERANDUPDATE_H
#define TIMERANDUPDATE_H

#include "common/globalvariable.h"

#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class TimerAndUpdate : public QTimer
{
    Q_OBJECT
public:
    explicit TimerAndUpdate(QObject *parent = 0);

signals:
    void Timer_Task_Quit(void);
    void Timer_Task_UpdateSQL(void);

public slots:
    void TestThread();

    void replyFinished(QNetworkReply *reply);

    void dealTask();

private:
    QTimer *timer_10s;
    void SetTimer(unsigned int time);
    void GetHttp(void);

    QNetworkAccessManager *accessManager;

    int timeCount_returnAndUpdate;
    int timeCount_sendNeedInfo;


};

#endif // TIMERANDUPDATE_H
