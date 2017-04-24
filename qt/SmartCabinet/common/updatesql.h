#ifndef UPDATESQL_H
#define UPDATESQL_H

#include <QObject>
#include "network/netcommunication.h"
#include "waitingpage.h"

class UpdateSQL : public QObject
{
    Q_OBJECT
public:
    explicit UpdateSQL(QObject *parent = 0);
    void Exec();
    void ShowWaitPage();
//    void GetCabinetInfo();
    bool GetWaitReturnList(int *user_Id);
    bool GetTaskList(int *user_Id, int task);
//    void GetServerTime();
//    void GetUpdateVersion();
//    void GetPositionStatusList();
    bool GetReagentStatusList(int *user_Id);
    bool GetCabinetPowerList();
    bool GetVersion();
    bool GetAgentiaType();
    bool GetAllAgentia(int *user_Id);
//    bool GetCabinetNo();

signals:

public slots:
private:
    WaitingPage *update_waitPage;
    NetCommunication *update_http;

    bool UnpackTaskList(QJsonDocument jd,int *task);
    bool UnpackAgentiaStatusList(QJsonDocument jd);
    bool UnpackPrivilegeTable(QJsonDocument jd);
    bool UnpackOccupiedAgentiaList(QJsonDocument jd);
    bool UnpackVersion(QJsonDocument jd);
    bool UnpackAgetiaType(QJsonDocument jd);
    bool UnpackAllAgetia(QJsonDocument jd);
//    bool UnpackCabinetNo(QJsonDocument jd);

    QString ReturnStatus(const int &agentiaStatus, QString &name);

};

#endif // UPDATESQL_H
