#include "netcommunication.h"
#include "globalvariable.h"
#include <QApplication>
#include "QtSql/QSqlQuery"




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
/***************************************************************/
//解析版本号
bool NetCommunication::UnpackVersion(QJsonDocument jd)
{
    QJsonObject analyze_Z = jd.object();
    QJsonValue s_json[5] = {0};
    bool s_success = false;
    QString armVersion = "";
    QString armDownLoad = "";

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    if (s_success)
    {
        QSqlQuery query;
        query.exec(QString("DELETE from T_temporary"));

        s_json[0] = analyze_Z["armVersion"].toString();
        armVersion = s_json[0].toString();

        s_json[1] = analyze_Z["armDownload"].toString();
        armDownLoad = s_json[1].toString();

        query.prepare("insert into T_temporary (versionNo,address)\
                values (?,?)");
        query.addBindValue(armVersion);
        query.addBindValue(armDownLoad);
        query.exec();


    }
    else
        return false;

    return true;
}



