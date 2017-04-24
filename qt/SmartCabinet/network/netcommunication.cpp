#include "netcommunication.h"
#include "common/globalvariable.h"
#include <QApplication>




NetCommunication::NetCommunication(QObject *parent) :
    QObject(parent)
{
    accessManager = new QNetworkAccessManager(this);
}


bool NetCommunication::PostHttp(const QString address, const QString &postStr, int s_time)
{
    qDebug() << postStr;
    QNetworkRequest *request = new QNetworkRequest();
    request->setUrl(QUrl(QString("http://121.43.159.215:3000/arm/%1").arg(address)));
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QByteArray postData = postStr.toUtf8();
    QNetworkReply *pReply = accessManager->post(*request, postData);

    QEventLoop loop;
    QTimer timer;
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start((s_time * 1000));//定时2s暂定
    loop.exec();

    if (timer.isActive())
    {
        timer.stop();
    }
    else
    {
        return false;
    }

#ifdef TEST

    qDebug() << QDateTime::currentDateTime() << "post send OK" <<    accessManager->networkAccessible();

#endif

    return replyFinished(pReply);
}

bool NetCommunication::replyFinished(QNetworkReply *reply)
{
#ifdef TEST

    qDebug() << QDateTime::currentDateTime() << "received from server";

#endif

    if (QNetworkReply::NoError == reply->error())
    {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QString all = codec->toUnicode(reply->readAll());
        QJsonDocument all_info = QJsonDocument::fromJson(all.toUtf8());

        ServerReply = all_info;

        if ("" != all)//无信息，代表通讯异常
        {
            NetWorkStatus = NETWORK_ONLINE;
        }
        else
        {
            NetWorkStatus = NETWORK_OFFLINE;
        }

#ifdef TEST
        qDebug() << all;
#endif
    }
    else
    {
        NetWorkStatus = NETWORK_OFFLINE;
        qDebug() << "http error here file:netcommunication  name:finished";
    }

#ifdef TEST

    qDebug() << QDateTime::currentDateTime() << "time out";

#endif

    reply->deleteLater();
    return NetWorkStatus;
}




/**********************************打包*****************************/
//打包登录请求
QString NetCommunication::PackageJson_login(const QString &userAccount, const QString &userPassWd)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("username", userAccount);
    json.insert("password",userPassWd);
    json.insert("cabinetNo", CABINETNO);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//打包机柜试剂状态列表请求
QString NetCommunication::PackageJson_agentiaStatusList(const int &user_Id, const QString &cabinetNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("cabinetNo", cabinetNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);
    return json_str;
}

//打包机柜任务列表请求
QString NetCommunication::PackageJson_taskList(const int &user_Id, const QString &cabinetNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("cabinetNo", cabinetNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//打包机柜权限列表请求
QString NetCommunication::PackageJson_privilegeTable(const QString &cabinetNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("cabinetNo", cabinetNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//打包获取未归还试剂列表请求
QString NetCommunication::PackageJson_occupiedAgentiaList(const int &user_Id, \
                                                          const QString &cabinetNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("cabinetNo", cabinetNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//打包上报任务完成——还
QString NetCommunication::PackageJson_giveBack(const int &user_Id, const int &agentiaId,\
                                               const QString &dose, const int &positionId)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("agentiaId", agentiaId);
    json.insert("dose", dose);
    json.insert("positionId", positionId);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//打包上报任务完成——取
QString NetCommunication::PackageJson_takeOut(const int &user_Id, const int &agentiaId, const int &positionId)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("agentiaId", agentiaId);
    json.insert("positionId", positionId);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//打包上报任务完成——任务完成
QString NetCommunication::PackageJson_taskComplete(const int &user_Id, const int &taskId)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("taskId", taskId);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//打包警报信息
QString NetCommunication::PackageJson_warningLog(const int &user_Id, const int &taskId, \
                                                 QString description)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("taskId", taskId);
    json.insert("description", description);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

QString NetCommunication::PackageJson_warningLog(const int &user_Id, QString description)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("description", description);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

QString NetCommunication::PackageJson_warningLog(QString description)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("description", description);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//打包禁用格子、抽屉信息
QString NetCommunication::PackageJson_disablePosition(const int &user_Id, \
                                                      const QString &cabinetId, \
                                                      const int &drawerNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("cabinetId", cabinetId);
    json.insert("drawerNo", drawerNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

QString NetCommunication::PackageJson_disablePosition(const int &user_Id, \
                                                      const QString &cabinetId, \
                                                      const int &drawerNo, \
                                                      const int &positionNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", user_Id);
    json.insert("cabinetId", cabinetId);
    json.insert("drawerNo", drawerNo);
    json.insert("positionNo", positionNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}


QString NetCommunication::PackageJson_version(const QString &cabinetId)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("cabinetId", cabinetId);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//试剂类型列表
QString NetCommunication::PackageJson_agentiaTypeList(const int &type, \
                                                      const int &goupId)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("type", type);
    json.insert("groupId", goupId);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//获取机柜信息
QString NetCommunication::PackageJson_CabinetNo(const QString &cabinet)
{
//    http_info->http_modelChoice = 0;
//    http_info->model_json = 0;
//    QNetworkRequest *request=new QNetworkRequest();
////    request->setUrl(QUrl(QString("http://localhost:3000/arm/initialInfo/%1").arg(CABINETNO)));
//    request->setUrl(QUrl(QString("http://121.43.159.215:3000/arm/initialInfo/%1").arg(CABINETNO)));
//    accessManager->get(*request);//通过发送数据，返回值保存在reply指针里.
}


//分配位置
QString NetCommunication::PackageJson_allocPosition(const QString &cabinetNo, const int &drawerNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("cabinetNo", cabinetNo);
    json.insert("drawerNo", drawerNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//上报入柜完成
QString NetCommunication::PackageJson_putIn(const int &userId, const int &agentiaTypeId, \
                                    const QString &bottleCapacity, const QString &dose, \
                                    const QString &expireDate, const int &positionId)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", userId);
    json.insert("agentiaTypeId", agentiaTypeId);
    json.insert("bottleCapacity", bottleCapacity);
    json.insert("dose", dose);
    json.insert("expireDate", expireDate);
    json.insert("positionId", positionId);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

//获取试剂列表
QString NetCommunication::PackageJson_agentiaList_drawer(const int &userId, \
                                          const QString &cabinetNo, \
                                          const int &drawerNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", userId);
    json.insert("cabinetNo", cabinetNo);
    json.insert("drawerNo", drawerNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}

QString NetCommunication::PackageJson_agentiaList(const int &userId, \
                                          const QString &cabinetNo)
{
    QJsonObject json;
    QJsonDocument document;
    QByteArray byte;

    json.insert("userId", userId);
    json.insert("cabinetNo", cabinetNo);

    document.setObject(json);
    byte = document.toJson(QJsonDocument::Compact);
    QString json_str(byte);

    return json_str;
}












