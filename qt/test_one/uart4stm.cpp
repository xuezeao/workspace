
/*
 * uart4Palm.c
 *
 *  Created on: 2015-8-20
 *      Author: siemon
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "uart4stm.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QByteArray>

int photosensor[2];
int alarm_position[15];
QSerialPort* myCom;

int Alarm_No[3][32];

void waitTaskInfo(int tim)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<tim)
    {
        QCoreApplication::processEvents();
    }
}

bool createSerialPort(const QString &portName, unsigned int baudRate)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
    }

    myCom= new QSerialPort();
    myCom->setPortName(portName);

    if(myCom->open(QIODevice::ReadWrite))
    {
        myCom->setFlowControl(QSerialPort::NoFlowControl);
        myCom->setBaudRate(baudRate);
        myCom->setDataBits(QSerialPort::Data8);
        myCom->setParity(QSerialPort::NoParity);
        myCom->setStopBits(QSerialPort::OneStop);
        return true;
    }
    else
        return false;
}


char uartCheckSum(char* data, int num) //校验码
{
    char sum = 0;
    int i = 0;
    for(i = 0; i < num; i++)
        sum += *(data + i);
    return sum;
}


void PackageSend(QSerialPort* uartfd,int DID, struct Package1 data1, struct Package2 data2)
{

    data1.Header[0] = 0x1C;
    data1.Header[1] = 0x47;
    data1.Header[2] = 0x45;
    data1.Header[3] = 0x42;

    data1.DevID[0] = DID;

    data1.CheckSum = uartCheckSum(&data1.DevID[0],4);
    int sendLen =  data1.ExtraDataLen[0];
    if(sendLen >40 )
        sendLen = 40;

    data2.ExtraData[sendLen-1] = uartCheckSum(&data2.ExtraData[0], sendLen-1);

    QByteArray dataMsg;
    dataMsg.resize(sizeof(data1)+sizeof(data2));
    memcpy(dataMsg.data(),&data1,9);
    memcpy(dataMsg.data()+9,&data2,sendLen);
    uartfd->write(dataMsg,9+sendLen);
}


int SetDrawerLock(QSerialPort* uartfd,int DID)//0x11 设置抽屉锁状态
{
    struct Package1 P1;
    P1.CmdID =CID_SET_DRAWER_LOCK;
    P1.ErrorCode = 0;
    P1.ExtraDataLen[0] = 0x02;

    struct Package2 P2;
    P2.ExtraData[0] = 1;

    PackageSend(uartfd,DID,P1, P2);
    return (0);
}

int SetAct(QSerialPort* uartfd,int DID,unsigned char *DataAct)//0x12 控制任务状态
{
    struct Package1 P1;
    P1.CmdID =CID_SET_ACT;
    P1.ErrorCode = 0;
    P1.ExtraDataLen[0] = 0x06;

    struct Package2 P2;
    for(int i=0;i<8;i++)
    {
        P2.ExtraData[i] = DataAct[i];
    }

    PackageSend(uartfd,DID,P1, P2);
    return (0);
}

int SendAct(QSerialPort* uartfd,int DID,int ActMode) //0x13 发送任务完成指令
{
    struct Package1 P1;
    P1.CmdID =CID_SEND_ACT;
    P1.ErrorCode = 0;
    P1.ExtraDataLen[0] = 0x02;

    struct Package2 P2;
    P2.ExtraData[0] = ActMode;

    PackageSend(uartfd,DID,P1, P2);
    return (0);
}


int RequestDrawerLock(QSerialPort* uartfd,int DID)//0x41 请求抽屉锁状态
{
    struct Package1 P1;
    P1.CmdID = CID_REQUEST_DRAWER_LOCK;
    P1.ErrorCode = 0;
    P1.ExtraDataLen[0] = 0x00;

    struct Package2 P2;

    PackageSend(uartfd,DID,P1, P2);
    return (0);
}

int RequestPhotosensor(QSerialPort* uartfd,int DID)//0x42 请求药品状态
{
    struct Package1 P1;
    P1.CmdID = CID_REQUEST_PHOTOSENSOR;
    P1.ErrorCode = 0;
    P1.ExtraDataLen[0] = 0x00;

    struct Package2 P2;

    PackageSend(uartfd,DID,P1, P2);
    return (0);
}

int RequestAlarm(QSerialPort* uartfd,int DID)//0xE0 请求报警信息
{
    struct Package1 P1;
    P1.CmdID = CID_ALARM;
    P1.ErrorCode = 0;
    P1.ExtraDataLen[0] = 0x00;

    struct Package2 P2;

    PackageSend(uartfd,DID,P1, P2);
    return (0);
}



int wait4GetDrawer(int DID)
{
    int i,j;
    for(i=0;i<3;i++)
    {
        RequestDrawerLock(myCom,DID);
        for(j=0;j<10;j++)
        {
            int rtnState = readData();
            if (rtnState == STATE_DRAWER_LOCK_CLOSE)
            {
                qDebug("CLOSE");
                return DRAWER_CLOSE;
            }
            else if(rtnState == STATE_DRAWER_LOCK_OPEN)
            {
                qDebug("OPEN");
                return DRAWER_OPEN;
            }
        }
    }
    return -1;
}

void SetDataAct(int *send_positionNo,int send_ActNum,unsigned char *DataAct)
{
        for(int ActNo = 0;ActNo < send_ActNum;ActNo++)
        {
            DataAct[send_positionNo[ActNo]/8] += (1 << ((send_positionNo[ActNo]%8)-1));
        }
}

int wait4SetAct(int DID,unsigned char *DataAct)//设置Act
{
    int i,j;
    for(i=0;i<3;i++)
    {
        SetAct(myCom,DID,DataAct);
        for(j=0;j<10;j++)
        {
            int rtnState = readData();
            if (rtnState == STATE_SET_ACT)
            {
                qDebug("Send Set Act OK!");
                return 0;
            }
        }
    }
    return -1;
}

int wait4SendAct(int DID,int ActMode)//发送任务完成指令
{
    int i,j;
    for(i=0;i<3;i++)
    {
        SendAct(myCom,DID,ActMode);
        for(j=0;j<10;j++)
        {
            int rtnState = readData();
            if (rtnState == STATE_SEND_ACT)
            {
                qDebug("Send Act OK!");
                return 0;
            }
        }
    }
    return -1;
}



void AnalyzeAlarm()    //解析警报信息
{
    int i,j;
    unsigned long Alarm;
    for(i = 0; i < 3; i++)
    {
        Alarm = 0;
        int Alarm_Num = 0;
        qDebug("E%d",i);
        for(j = 0; j < 4; j++)
        {
            Alarm += alarm_position[j+1+i*5]<<(j)*8;
        }
        for(j = 0; j < 32; j++)
        {
            if(Alarm & 0x80000000)
            {
                Alarm_No[i][Alarm_Num] = 32-j;
                qDebug("%d",Alarm_No[i][Alarm_Num++]);
            }
            Alarm <<= 1;
        }
    }
}


int wait4Alarm(int DID)
{
    int i,j;
    for(i=0;i<3;i++)
    {
        RequestAlarm(myCom,DID);
        for(j=0;j<10;j++)
        {
            int rtnState = readData();
            if (rtnState == STATE_ALARM)
            {
                qDebug("Get ALARM");
                memset(Alarm_No[0],0,32);
                memset(Alarm_No[1],0,32);
                memset(Alarm_No[2],0,32);
                AnalyzeAlarm();
                if(Alarm_No[0][0] != 0)
                {
                    wait4SendAct(0x01,1);
                    return 0;
                }
                else if(Alarm_No[2][0] != 0)
                {
                    wait4SendAct(0x01,1);
                    return 1;
                }
                else
                {
                    return 2;
                }
            }
        }
    }
    return -1;
}




int wait4OpenDrawer(int DID)        //0x11
{
    int i,j;
    for(i=0;i<3;i++)
    {
        SetDrawerLock(myCom,DID);
        for(j=0;j<10;j++)
        {
            waitTaskInfo(3000);
            int rtnState = readData();
            if (rtnState == STATE_SET_DRAWER_LOCK)
            {
                qDebug("Send Open Drawer Command OK!");
                return 0;
            }
        }
    }
    return -1;
}

int IntoDrawer(int DID)     //开锁
{
    int i;
    int drawerState = wait4GetDrawer(DID);
    if(drawerState == DRAWER_CLOSE)
    {
        for(i = 0; i < 2; i++)
        {
            if(wait4OpenDrawer(DID))
                return -1;
            drawerState = wait4GetDrawer(DID);
            if(drawerState == -1)
                return -1;
            else if(drawerState == DRAWER_OPEN)
                return 0;
            else
            {
                if(i == 3)
                    return -1;
            }
        }
        return -1;
    }
    else if(drawerState == -1)
        return -1;
    else
        return 0;
}

int readData()
{
    int i,j;
    QByteArray alldata;
    for(j=0;j<30;j++)
    {
        waitTaskInfo(2);
        if(myCom->bytesAvailable()>=9)
        {

            alldata.append(myCom->readAll());
            qDebug("Data recv %d.", alldata.size());
            for(i=0;i<(alldata.size()-4);i++)
            {
                if((alldata.at(i) == 0x1b)&&(alldata.at(i+1) == 0x46)&&(alldata.at(i+2) == 0x44)&&(alldata.at(i+3) == 0x41))
                    break;
            }
            int pHead = i;
            if((alldata.size()-pHead) < 9)
                continue;
            else
            {
                unsigned char CID = 0;
                int exDataLen = 0;
                unsigned char checkSum = 0;
                for(i = pHead+4; i < pHead+8; i++)
                    checkSum += alldata.at(i);
                if((unsigned char)(alldata.at(i)) != checkSum)
                {
                    qDebug("P1 Check Sum Error!\n");
                    alldata.clear();
                    return STATE_ERROR;
                }
                else
                {
                    qDebug("P1 Check Sum Confirmed!\n");
                    CID=(unsigned char)(alldata.at(pHead+5));
                    exDataLen = (unsigned char)(alldata.at(pHead+6));
                    //有扩展帧
                    if(exDataLen>0)
                    {
                        qDebug("Extra data length: %d\n", exDataLen);
                        checkSum = 0;
                        while(myCom->bytesAvailable()>0)
                        {
                            waitTaskInfo(2);
                            alldata.append(myCom->readAll());
                        }
                        if(alldata.count() < pHead+exDataLen+9)
                        {
                            qDebug("CID ERROR1");
                            continue;
                        }
                        for(i = pHead+9;i<pHead+exDataLen+8;i++)
                            checkSum += alldata.at(i);
                        if((unsigned char)(alldata.at(pHead+exDataLen+8)) == checkSum)
                        {
                            if(CID == CID_REQUEST_DRAWER_LOCK) //0x41
                            {
                                if(alldata.at(pHead+9)==0)
                                {
                                    qDebug("Door1 Lock CLOSE!");
                                    alldata.clear();
                                    return STATE_DRAWER_LOCK_CLOSE;
                                }
                                else
                                {
                                    qDebug("Door1 Lock OPEN!");
                                    alldata.clear();
                                    return STATE_DRAWER_LOCK_OPEN;
                                }

                            }
                            else if(CID==CID_REQUEST_PHOTOSENSOR) //0x42
                            {
                                photosensor[0]=(unsigned char)(alldata.at(pHead+9));
                                photosensor[1]=(unsigned char)(alldata.at(pHead+10));
                                alldata.clear();
                                qDebug("Request Photosensor!");
                                return STATE_REQUEST_PHOTOSENSOR;
                            }
                            else if(CID==CID_ALARM)                 //0xE0
                            {
                                memset(alarm_position,0,15);
                                for(int i=0;i<15;i++)
                                {
                                    alarm_position[i]=(unsigned char)(alldata.at(pHead+9+i));
                                }
                                alldata.clear();
                                qDebug("Request alarm position!");
                                return STATE_ALARM;
                            }
                            else
                            {
                                alldata.clear();
                                qDebug("CID ERROR2");
                                return STATE_ERROR;
                            }

                        }
                        else
                        {
                            alldata.clear();
                            qDebug("P2 Check Sum Error: %x\n", checkSum);
                            return STATE_ERROR;
                        }
                    }
                    //没有扩展帧
                    else
                    {
                        if(CID == CID_SET_DRAWER_LOCK )                 //0x11
                        {
                            alldata.clear();
                            qDebug("CID_SET_DRAWER_LOCK");
                            return STATE_SET_DRAWER_LOCK;
                        }
                        else if(CID==CID_SET_ACT)                       //0x12
                        {
                            alldata.clear();
                            qDebug("CID_SET_ACT");
                            return STATE_SET_ACT;
                        }
                        else if(CID==CID_SEND_ACT)                      //0x13
                        {
                            alldata.clear();
                            qDebug("CID_SET_ACT");
                            return STATE_SEND_ACT;
                        }
                        else
                        {
                            alldata.clear();
                            qDebug("CID ERROR3");
                            return STATE_ERROR;
                        }
                    }
                }
            }
        }
    }
    return STATE_ERROR;
}
