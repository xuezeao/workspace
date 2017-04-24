/*
 * 定时器：5S
 *
 * */
#include "serialportcontrol.h"
#include "common/uart4stm.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
#include <QCoreApplication>


SerialPortControl::SerialPortControl(QObject *parent) :
    QObject(parent)
{
    isClickReturnPB = false;

    temporaryLock = 0;
}

SerialPortControl::~SerialPortControl()
{
    qDebug() << "serialPort_already destory";
}

void SerialPortControl::HandleFlow(int DID, int send_ActNum)
{
    bool isContinueExecute = false;
    //查询锁的状态
//    if (RequestLockStauts(DID))//CLOSE 0 OPEN 1
//        isContinueExecute = true;
//    else
//    {
//        if (Unlock(DID))
//            isContinueExecute = true;
//        else
//            Task_Error(ACT_INTODRAWER_FAIL);
//    }

    if (temporaryLock != DID)
    {
        Unlock(DID);
        temporaryLock = DID;
    }
    isContinueExecute = true;

    if (isContinueExecute)
    {
        if (Send_Act(DID, send_ActNum))
        {
            //默认不退出
            isQuitLoopRequestAgentiaStatus = false;

            for (int i = 0; i < 20; i++)
            {
                if (! IsQuitCheckTask(DID))
                    i = 0;
                else
                    break;
            }

            if (! TaskEnd(DID))
            {
                for (int i = 0; i < 3; i++)
                {
                    if (TaskEnd(DID))
                        break;
                }
            }
            //任务完成
            Task_Error(ACT_TASK_OK);
        }
        else
        {
            Task_Error(ACT_SENDTASK_FAIL);
        }
    }

    if (isClickReturnPB)
    {
        DestroyProcess();
    }
}

//查询锁的状态
bool SerialPortControl:: RequestLockStauts(int DID)
{
//    int lockStatus = wait4GetDrawer(DID);//CLOSE 0 OPEN 1
    int lockStatus = 0;
    if (1 == lockStatus)
    {
        return true;
    }
    return false;
}
//开锁
bool SerialPortControl::Unlock(int DID)
{
//    int status_L = IntoDrawer(DID);
    opendrawer(DID);
    qDebug()<<" Unlock";
    int status_L = 0 ;
    if (0 == status_L)
    {
        return true;
    }
    return false;
}
//发指令send_ActNum整型数组
bool SerialPortControl::Send_Act(int DID, int &send_ActNum)
{
    int taskSend_S = wait4SetAct(DID, send_ActNum, send_positionNo_G);

    if (0 == taskSend_S)
    {
        return true;
    }
    return false;
}

//-1：错误 1：未完成 0：完成
bool SerialPortControl::IsQuitCheckTask(int &DID)
{
    int status = Requset_Alarm(DID);
    waitTaskInfo(2000);
    if (0 == status)
        return false;
    else if (1 == status)
    {
        if (! isQuitLoopRequestAgentiaStatus)
            IsQuitCheckTask(DID);
        else
            return true;
    }
    else if (2 == status)
    {
        isQuitLoopRequestAgentiaStatus = true;
        return true;
    }
    else if (-1 == status)
    {
        isQuitLoopRequestAgentiaStatus = true;
        return true;
    }

    return false;
}

//查询试剂状态
//-1：错误 1：未完成 0：完成
int SerialPortControl::Requset_Alarm(int &DID)
{
    int status = wait4Alarm(DID);

    if (0 == status)
        emit ReplyCurrentAgentiaStatus(TASK_ERROR);
    else if (1 == status)
        emit ReplyCurrentAgentiaStatus(TASK_HAVENOTEXEC);
    else if (2 == status)
        emit ReplyCurrentAgentiaStatus(TASK_OVER);
    else if (-1 == status)
        emit ReplyCurrentAgentiaStatus(TASK_NOTREPLY);

    return status;
}

//完成任务
bool SerialPortControl::TaskEnd(int &DID)
{
    //ActMode 暂时为0即可：任务完成
    int status = wait4SendAct(DID, 0);

    if (0 == status)
        return true;
    else
        Task_Error(ACT_WORKVOER_FAIL);
    return false;
}

void SerialPortControl::CreateSerialPort()
{
    if (1 == checkSerialPort())
        closeSerialPort();

    qDebug()<<"serialPort thread:  "<< QThread::currentThreadId();

    if(!(createSerialPort(38400)))//打开串口38400
        emit Task_Error(ACT_CREATESERIALPORT_FAIL);
    else
        emit Task_Error(ACT_CREATESERIALPORT_SUCCESS);
}

void SerialPortControl::DestroyProcess()
{
    closeSerialPort();
    this->deleteLater();
}

void SerialPortControl::wait(int time)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<time)
    {
        QCoreApplication::processEvents();
    }
}

void SerialPortControl::ReturnLockStatus(int DID)
{
//    int lockStatus = wait4GetDrawer(DID);//CLOSE 0 OPEN 1
    int lockStatus = 0;
    if (1 == lockStatus)
        emit ReturnLock(DRAWEROPEN);
    else
        emit ReturnLock(DRAWERCLOSE);
}

