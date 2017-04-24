#ifndef NETCOMMUNICATION_H
#define NETCOMMUNICATION_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QtNetwork>
#include <QNetworkRequest>
#include <QUrl>
#include <QByteArray>
#include <QDebug>
#include <QDateTime>

class NetCommunication : public QObject
{
    Q_OBJECT
public:
    explicit NetCommunication(QObject *parent = 0);
    ~NetCommunication(){} 

    QJsonDocument ServerReply;

    bool PostHttp(const QString address, const QString &postStr, int s_time);
    void GetHttp(void);
    QString PackageJson_login(const QString &userAccount, const QString &userPassWd);
    QString PackageJson_agentiaStatusList(const int &user_Id, const QString &cabinetNo);
    QString PackageJson_taskList(const int &user_Id, const QString &cabinetNo);
    QString PackageJson_privilegeTable(const QString &cabinetNo);
    QString PackageJson_occupiedAgentiaList(const int &user_Id, const QString &cabinetNo);

    QString PackageJson_giveBack(const int &user_Id, const int &agentiaId,\
                                 const QString &dose, const int &positionId);

    QString PackageJson_takeOut(const int &user_Id, const int &agentiaId, const int &positionId);
    QString PackageJson_taskComplete(const int &user_Id, const int &taskId);

    //禁用格子、抽屉
    QString PackageJson_disablePosition(const int &user_Id, const QString &cabinetId,\
                                        const int &drawerNo, const int &positionNo);
    QString PackageJson_disablePosition(const int &user_Id, const QString &cabinetId,\
                                        const int &drawerNo);
    //报警信息
    QString PackageJson_warningLog(const int &user_Id, const int &taskId, QString description);
    QString PackageJson_warningLog(const int &user_Id, QString description);
    QString PackageJson_warningLog(QString description);

    //版本信息
    QString PackageJson_version(const QString &cabinetId);

    //试剂类型列表
    QString PackageJson_agentiaTypeList(const int &type, const int &goupId);

    //获取柜子信息
    QString PackageJson_CabinetNo(const QString &cabinet);

    //请求分配位置
    QString PackageJson_allocPosition(const QString &cabinetNo, const int &drawerNo);

    //上报入柜完成
    QString PackageJson_putIn(const int &userId, const int &agentiaTypeId, const QString &bottleCapacity,\
                      const QString &dose, const QString &expireDate, const int &positionId);

    //获取所有试剂列表
    QString PackageJson_agentiaList_drawer(const int &userId, const QString &cabinetNo,\
                            const int &drawerNo);

    QString PackageJson_agentiaList(const int &userId, const QString &cabinetNo);
signals:

public slots:

private:
    QNetworkAccessManager *accessManager;


    bool replyFinished(QNetworkReply *reply);


};

#endif // NETCOMMUNICATION_H
