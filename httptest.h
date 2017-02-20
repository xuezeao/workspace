#ifndef HTTPTEST_H
#define HTTPTEST_H

#include <QObject>
#include <QByteArray>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

/**************json使用******************/
#include <QSqlTableModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

class HttpTest : public QObject
{
    Q_OBJECT
public:
    explicit HttpTest(QObject *parent = 0);


signals:

public slots:
    void GetHttpa(void);
    void finished(QNetworkReply *reply);
private:
    QNetworkAccessManager *manager;


};

#endif // HTTPTEST_H
