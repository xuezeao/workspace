#include "delaytime.h"
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QDebug>
#include <QThread>

DelayTime::DelayTime(QObject *parent) :
    QObject(parent)
{

}

void DelayTime::waitTaskInfo(int tim)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<tim)
    {
        QCoreApplication::processEvents();
    }
}
