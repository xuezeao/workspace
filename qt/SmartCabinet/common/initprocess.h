#ifndef INITPROCESS_H
#define INITPROCESS_H

#include <QObject>

#include <QDebug>
#include <QMessageBox>

#include "ui/common/dg_loginpage.h"
#include "common/threadcontrol.h"
#include "network/netcommunication.h"
#include "common/Sql_Setting.h"
#include "common/updatesql.h"

class Initprocess : public QObject
{
    Q_OBJECT
public:
    explicit Initprocess(QObject *parent = 0);

    void ReadConfig();
    void CreateSql();
    void OpenSerialPort();
    void CreateThread();
    void StartThread();
    void Upgrade();
    void ShowLoginPage();
    void OpenKey();

signals:


public slots:
    void UpdateLocalSql();
    void Quit_Login();
private:
    NetCommunication *init_http_updateSheet;
    Dg_LoginPage *init_loginPage;
    ThreadControl *init_threadControl;
    UpdateSQL *updateSQL;

    bool UnpackVersion(QJsonDocument jd, QString *armVersion, QString *armUrl);



};

#endif // INITPROCESS_H
