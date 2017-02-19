#include "wthread.h"

WThread::WThread(QObject *parent) :
    QThread(parent)
{


}

void WThread::run()
{
    httpgg = new HttpGP;
    httpgg->GetHttp(2);
    exec();
}
