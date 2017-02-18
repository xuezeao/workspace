#ifndef UART4STM_H
#define UART4STM_H
#include <QSerialPort>
#include <QSerialPortInfo>

/*
 * uart4Palm.h
 *
 *  Created on: 2015-8-18
 *      Author: siemon
 */
//#define SERIALPORT   "ttyUSB0"         //ARM
#define SERIALPORT   "com5"              //PC
#define CONTROL_BOARD_HEAD_SEND	0x1C474542
#define CONTROL_BOARD_HEAD_RECV	0x1B464441
#define CONTROL_BOARD_HEAD_RESP 0xEB90EB90

#define STM8_0_DEVID		0xFF
#define STM8_1_DEVID		0x01

#define BUFFER_SIZE		512

#define DRAWER_OPEN 1
#define DRAWER_CLOSE  0

#define CID_SET_DRAWER_LOCK                 0x11
#define CID_SET_ACT                         0x12
#define CID_SEND_ACT                        0x13

#define CID_REQUEST_DRAWER_LOCK             0x41
#define CID_REQUEST_PHOTOSENSOR             0x42

#define CID_ALARM                           0xE0



enum
{
    STATE_SET_DRAWER_LOCK,                  //0x11 设置抽屉锁状态
    STATE_SET_ACT,                          //0x12 设置Act状态
    STATE_SEND_ACT,                         //0x13

    STATE_DRAWER_LOCK_OPEN,
    STATE_DRAWER_LOCK_CLOSE,                //0x41 请求抽屉锁状态
    STATE_REQUEST_PHOTOSENSOR,              //0x42 请求请求光电状态状态

    STATE_ALARM,

    STATE_NONE,
    STATE_ERROR
};

struct Package1
{
    char Header[4];
    char DevID[1];
    char CmdID;
    char ExtraDataLen[1];
    char ErrorCode;
    char CheckSum;
};

struct Package2
{
    char ExtraData[256];
    char ExCheckSum;
};

extern int STATE_RTN ;
void waitTaskInfo(int tim);
bool createSerialPort(const QString &portName, unsigned int baudRate);

void PackageSend(QSerialPort* uartfd,int DID, struct palmPackage1 data1, struct palmPackage2 data2);

int SetDrawerLock(QSerialPort* uartfd,int DID, int Mode);                       //0x11 设置抽屉锁状态
int SetAct(QSerialPort* uartfd,int DID,unsigned char *DataAct);                 //0x12 设置Act状态
int SendAct(QSerialPort* uartfd,int DID,int ActMode);                            //0x13 发送任务完成指令

int RequestDrawerLock(QSerialPort* uartfd,int DID);                             //0x41 请求抽屉锁状态
int RequestPhotosensor(QSerialPort* uartfd,int DID);                            //0x42 请求请求光电状态状态

int RequestAlarm(QSerialPort* uartfd,int DID);                                  //0xE0 请求报警信息

int wait4GetDrawer(int DID);

void SetDataAct(int *send_positionNo,int send_ActNum,unsigned char *DataAct);
int wait4SetAct(int DID,unsigned char *DataAct);
int wait4SendAct(int DID,int ActMode);
int wait4Alarm(int DID);
void AnalyzeAlarm();

int wait4OpenDrawer(int DID);
int IntoDrawer(int DID);                                                        //开锁

int readData();


#endif // UART4STM_H
