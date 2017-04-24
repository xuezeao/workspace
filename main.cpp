#include "mainchoice.h"
#include "waitingpage.h"
#include "dialog_controlthread.h"

#include <QApplication>
#include <QTimer>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    MainChoice w;
//    WaitingPage w;
    Dialog_ControlThread w;
    w.show();
    qDebug() << "main thread Id: "<<QThread::currentThreadId();

    return a.exec();
}


