#include "httptest.h"
#include <QDebug>

HttpTest::HttpTest(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)),Qt::QueuedConnection);
}

void HttpTest::GetHttpa()
{
        QNetworkRequest *request = new QNetworkRequest;
//    request->setUrl(QUrl(QString("http://localhost:3000/arm/initialInfo/%1").arg(CABINETNO)));
    request->setUrl(QUrl(QString("http://121.43.159.215:3000/arm/%1").arg("serverTime")));
    manager->get(*request);//通过发送数据，返回值保存在reply指针里.
    qDebug()<<"1111111112222222222";

}

void HttpTest::finished(QNetworkReply *reply)
{
    qDebug()<<"123123123123123";
    if (reply->error() == QNetworkReply::NoError)
    {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QString all = codec->toUnicode(reply->readAll());
        QJsonDocument all_info = QJsonDocument::fromJson(all.toUtf8());

        QString    s_str[11]    = {0};//save string
        QJsonValue s_json[11]   = {0};//save jsonvalue
        QJsonObject analyze_Z = all_info.object();

        bool s_success    = false;


        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            s_json[1] = analyze_Z["serverTime"].toString();
            s_str[1] = s_json[1].toString();
            qDebug()<<s_str[1]<<"111111111111111111";
        }


    }
}
