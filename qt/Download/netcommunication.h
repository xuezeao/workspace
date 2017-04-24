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
#include <QMessageBox>

class NetCommunication : public QObject
{
    Q_OBJECT
public:
    explicit NetCommunication(QObject *parent = 0);
    ~NetCommunication(){}

    QJsonDocument ServerReply;

    bool PostHttp(const QString address, const QString &postStr, int s_time);
    void GetHttp(void);

    //版本信息
    QString PackageJson_version(const QString &cabinetId);
    bool UnpackVersion(QJsonDocument jd);


signals:

public slots:

private:
    QNetworkAccessManager *accessManager;


    bool replyFinished(QNetworkReply *reply);


};

#endif // NETCOMMUNICATION_H
