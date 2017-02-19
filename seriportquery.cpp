#include "seriportquery.h"
#include "delaytime.h"
#include <QDebug>
#include <QThread>
SeriportQuery::SeriportQuery(QObject *parent) :
    QObject(parent)
{
}

void SeriportQuery::requestSTM()
{

    DelayTime a;
    while (true)
    {
        qDebug() << "loop thread id: "<< QThread::currentThreadId();
        qDebug() <<"11-----";
        a.waitTaskInfo(1000);
    }
}
