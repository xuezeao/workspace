#include "global_Vailable.h"
#include <QElapsedTimer>
#include <QCoreApplication>

void waitOtherThing(int a)
{
    QElapsedTimer t;//主程序等待
    t.start();
     while(t.elapsed() < a)
     {
         QCoreApplication::processEvents();
     }
}
