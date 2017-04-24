#ifndef GLOBALVARIABLE_H
#define GLOBALVARIABLE_H

#include <QString>

#define TEST //test mode
//#define CABINETNO "AABBCCDD"

#define OPEN_THREAD 1
#define CLOSE_THREAD 0

#define NETWORK_OFFLINE 0
#define NETWORK_ONLINE  1


extern int NetWorkStatus;

const QString localFileName = "/home/x/SmartCabinet";
const QString serverIp = "http://121.43.159.215:3000";
const QString goalFile = "build-untitled-Desktop_Qt_5_7_1_GCC_64bit-Debug";
extern QString CABINETNO;

#endif // GLOBALVARIABLE_H
