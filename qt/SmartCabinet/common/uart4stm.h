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

#define CONTROL_BOARD_HEAD_SEND	0x1C474542
#define CONTROL_BOARD_HEAD_RECV	0x1B464441
#define CONTROL_BOARD_HEAD_RESP 0xEB90EB90

#define STM8_0_DEVID		0xFF
#define STM8_1_DEVID		0x01

#define BUFFER_SIZE		512

#define DRAWER_OPEN 1
#define DRAWER_CLOSE  0


#define CID_SET_ACT                         0x12
#define CID_SEND_ACT                        0x13
#define CID_ALARM                           0xE0

#define CID_SET_DRAWER_LOCK                 0x01
#define CID_REQUEST_DRAWER_LOCK             0x00
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

struct Package3
{
    char drawer_Header[2];
    char drawer_DevID[1];
    char drawer_CmdID;
    char drawer_Date[3];
};


extern int STATE_RTN ;
void waitTaskInfo(int tim);
extern bool createSerialPort(unsigned int baudRate);
void closeSerialPort();
int checkSerialPort();//1 打开 0 关闭 -1 异常

void PackageSend(QSerialPort* uartfd,int DID, struct palmPackage1 data1, struct palmPackage2 data2);

int SetAct(QSerialPort* uartfd,int DID,unsigned char *DataAct);                 //0x12 设置Act状态
int SendAct(QSerialPort* uartfd,int DID,int ActMode);                            //0x13 发送任务完成指令

int RequestAlarm(QSerialPort* uartfd,int DID);                                  //0xE0 请求报警信息

int wait4GetDrawer(int DID);

int wait4SetAct(int DID,int send_ActNum,int *send_positionNo);//设置Act 抽屉号，个数，具体位置
int wait4SendAct(int DID,int ActMode);//1:success  0:fail
int wait4Alarm(int DID);//报警查询 0：错误 1：未完成 2：完成
void AnalyzeAlarm();

int readData();
/****************************************************************************************/
void drawer_PackageSend(QSerialPort* uartfd,int DID, struct Package3 data3);
int DrawerLock(QSerialPort* uartfd,int CID,int DID,char DataAct[3]);
int opendrawer(int DrawerNo);


extern int Alarm_No[3][64];
#endif // UART4STM_H
