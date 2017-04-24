
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
int alarm_position[27];
QSerialPort* myCom;
QByteArray alldata;

int Alarm_No[3][64];

void waitTaskInfo(int tim)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<tim)
    {
        QCoreApplication::processEvents();
    }
}

extern bool createSerialPort(unsigned int baudRate)
{
    myCom= new QSerialPort();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        myCom->setPortName(info.portName());
    }

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

void closeSerialPort()
{
    myCom->close();
    qDebug("串口关闭");
}

int checkSerialPort()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
       if(info.isBusy())
       {
           qDebug("busy: yes");
           return 1;
       }
       else
       {
           qDebug("busy: NO");
           return 0;
       }
    }
    return -1;
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

int wait4SetAct(int DID,int send_ActNum,int *send_positionNo)//设置Act
{
    int i,j;
    unsigned char DataAct[8] = {0};
    for(int ActNo = 0;ActNo < send_ActNum;ActNo++)
    {
        DataAct[(send_positionNo[ActNo]-1)/8] += (1 << ((send_positionNo[ActNo]-1)%8));
    }
    for(i=0;i<3;i++)
    {
        if(myCom->bytesAvailable()>0)
            myCom->readAll();
        SetAct(myCom,DID,DataAct);
        waitTaskInfo(50);
        int rtnState = readData();
        if (rtnState == STATE_SET_ACT)
        {
            qDebug("Send Set Act OK!");
            return 0;
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
        waitTaskInfo(50);
        int rtnState = readData();
        if (rtnState == STATE_SEND_ACT)
        {
            qDebug("Send Act OK!");
            return 0;
        }
    }
    return -1;
}



void AnalyzeAlarm()    //解析警报信息
{
    int i,j;
    unsigned long long Alarm;
    for(i = 0; i < 3; i++)
    {
        Alarm = 0;
        int Alarm_Num = 0;
        qDebug("E%d",i);
        for(j = 0; j < 8; j++)
        {
            Alarm += alarm_position[j+1+i*9]<<(j)*8;
        }
        for(j = 0; j < 64; j++)
        {
            if(Alarm & 0x8000000000000000)
            {
                Alarm_No[i][Alarm_Num] = 64-j;
                qDebug("%d",Alarm_No[i][Alarm_Num++]);
            }
            Alarm <<= 1;
        }
    }
}


int wait4Alarm(int DID)

{
    int i;
    for(i=0;i<3;i++)
    {
        RequestAlarm(myCom,DID);
        waitTaskInfo(50);
            int rtnState = readData();
            if (rtnState == STATE_ALARM)
            {
                qDebug("Get ALARM");
                memset(Alarm_No[0],0,64);
                memset(Alarm_No[1],0,64);
                memset(Alarm_No[2],0,64);
                AnalyzeAlarm();
                if(Alarm_No[0][0] != 0)
                {
                    return 0;
                }
                else if(Alarm_No[2][0] != 0)
                {
                    return 1;
                }
                else
                {
                    return 2;
                }
            }
    }
    return -1;
}

int readData()
{
    int i,j;
    alldata.clear();
    for(j=0;j<20;j++)
    {
        waitTaskInfo(50);
        if(myCom->bytesAvailable()>=9)
        {
            QByteArray frameHead;
            frameHead.append(0x1B);
            frameHead.append(0x46);
            frameHead.append(0x44);
            frameHead.append(0x41);
            alldata.append(myCom->readAll());
            qDebug("Data recv %d.", alldata.size());
            if(!alldata.contains(frameHead))
            {
                qDebug("Invalid frame head!\n");
                alldata.clear();
                return STATE_ERROR;
            }
            int pHead = alldata.indexOf(frameHead,0);

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
                        waitTaskInfo(100);
                        if(myCom->bytesAvailable()>0)
                            alldata.append(myCom->readAll());
                        if(alldata.count() < pHead+exDataLen+9)
                        {
                            qDebug("CID ERROR1");
                            return STATE_ERROR;
                        }
                        qDebug() << alldata<< "      alldata------";
                        for(i = pHead+9;i<pHead+exDataLen+8;i++)
                            checkSum += alldata.at(i);
                        if((unsigned char)(alldata.at(pHead+exDataLen+8)) == checkSum)
                        {
                            if(CID == CID_REQUEST_DRAWER_LOCK) //0x41
                            {
                                if(alldata.at(pHead+9)==0)
                                {
                                    qDebug("Door1 Lock CLOSE!");
                                    return STATE_DRAWER_LOCK_CLOSE;
                                }
                                else
                                {
                                    qDebug("Door1 Lock OPEN!");
                                    return STATE_DRAWER_LOCK_OPEN;
                                }

                            }
                            else if(CID==CID_ALARM)                 //0xE0
                            {
                                memset(alarm_position,0,27);
                                for(int i=0;i<27;i++)
                                {
                                    alarm_position[i]=(unsigned char)(alldata.at(pHead+9+i));
                                }
                                qDebug("Request alarm position!");
                                return STATE_ALARM;
                            }
                            else
                            {
                                qDebug("CID ERROR2");
                                return STATE_ERROR;
                            }

                        }
                        else
                        {
                            qDebug("P2 Check Sum Error: %x\n", checkSum);
                            return STATE_ERROR;
                        }
                    }
                    //没有扩展帧
                    else
                    {
                        if(CID == CID_SET_DRAWER_LOCK )                 //0x11
                        {
                            qDebug("CID_SET_DRAWER_LOCK");
                            return STATE_SET_DRAWER_LOCK;
                        }
                        else if(CID==CID_SET_ACT)                       //0x12
                        {
                            qDebug("CID_SET_ACT");
                            return STATE_SET_ACT;
                        }
                        else if(CID==CID_SEND_ACT)                      //0x13
                        {
                            qDebug("CID_SET_ACT");
                            return STATE_SEND_ACT;
                        }
                        else
                        {
                            qDebug("CID ERROR3");
                            return STATE_ERROR;
                        }
                    }
                }
        }
    }
    return STATE_ERROR;
}

/*************************************开锁**************************************************************/
void drawer_PackageSend(QSerialPort* uartfd,int DID, struct Package3 data3)
{

    data3.drawer_Header[0] = 0xAA;
    data3.drawer_Header[1] = 0x55;
    data3.drawer_DevID[0] = DID;
    QByteArray dataMsg;
    dataMsg.resize(sizeof(data3));
    memcpy(dataMsg.data(),&data3,8);
    uartfd->write(dataMsg,8);
}

int DrawerLock(QSerialPort* uartfd,int CID,int DID,unsigned char *DataAct)
{
    struct Package3 P3;
    P3.drawer_CmdID =CID;
    for(int i = 0; i < 3; i++)
    {
        P3.drawer_Date[i] = DataAct[i];
    }
    drawer_PackageSend(uartfd,DID,P3);
    return (0);
}

int opendrawer(int DrawerNo)
{

    myCom->setBaudRate(9600);
    waitTaskInfo(100);
    int CID = CID_SET_DRAWER_LOCK;
    unsigned char DataAct[3] = {0};
    DataAct[0] = (1 << (DrawerNo-1));
    DrawerLock(myCom,CID,0,DataAct);
    waitTaskInfo(500);
    myCom->setBaudRate(38400);
    waitTaskInfo(100);

}

