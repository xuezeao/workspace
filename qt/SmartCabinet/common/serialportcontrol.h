#ifndef SERIALPORTCONTROL_H
#define SERIALPORTCONTROL_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include "common/globalvariable.h"
#include "common/uart4stm.h"
#include <QMessageBox>
#include <QTimer>



class SerialPortControl : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortControl(QObject *parent = 0);
    ~SerialPortControl();

signals:
    void Task_Error(int status);
    void ReplyCurrentAgentiaStatus(int status);
    void ReturnLock(int DID);

public slots:
    void HandleFlow(int DID,int send_ActNum);
    void CreateSerialPort();
    void ReturnLockStatus(int DID);
    void DestroyProcess();

private:

    int temporaryLock;

    void wait(int timer);
    bool RequestLockStauts(int DID);
    bool Unlock(int DID);
    bool Send_Act(int DID,int &send_ActNum);
    int  Requset_Alarm(int &DID);
    bool IsQuitCheckTask(int &DID);
    bool TaskEnd(int &DID);

};

#endif // SERIALPORTCONTROL_H
