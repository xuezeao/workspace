#include "initprocess.h"
#include "common/uart4stm.h"
#include "network/netcommunication.h"
#include <QDebug>
#include <QSqlQuery>
#include "inputnew/frminputnew.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QProcess>

Initprocess::Initprocess(QObject *parent) :
    QObject(parent)
{
    init_threadControl = new ThreadControl(this);
    updateSQL = new UpdateSQL(this);
    connect(init_threadControl, SIGNAL(Update_SQL()), this, SLOT(UpdateLocalSql()));
    connect(init_threadControl, SIGNAL(Close_ALLPage()), this, SLOT(Quit_Login()));
}


void Initprocess::ReadConfig()
{
    QSqlQuery query;
    query.exec(QString("select * from T_Version"));
    query.last();
    qDebug()<<query.value(0).toString();
    qDebug()<<query.value(1).toString();

    CABINETNO = query.value(1).toString();
    VERSION = query.value(0).toString();
}

void Initprocess::CreateSql()
{
    if(!createConnection())//创建数据库，注意测试阶段数据库一启动便会初始化具体Sql_Setting文件
    {
        qDebug()<<"data base already create";
    }
}

void Initprocess::OpenSerialPort()
{

}

void Initprocess::CreateThread()
{
    init_threadControl->CreateThread();
}

void Initprocess::StartThread()
{
    init_threadControl->ControlTimerThread(OPEN_THREAD);
}

void Initprocess::ShowLoginPage()
{
    init_loginPage = new Dg_LoginPage;
    init_loginPage->show();
    connect(init_loginPage, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

void Initprocess::UpdateLocalSql()
{
    updateSQL->GetCabinetPowerList();
//    updateSQL->GetReagentStatusList(&userId);
}

void Initprocess::Quit_Login()
{
    init_loginPage->Close_ActivePage();
}

void Initprocess::OpenKey()
{
//    frmInputNew::Instance()->init("bottom", "black", 12, 10, 700, 230, 20, 20, 6, 45);
}

void Initprocess::Upgrade()
{
    NetCommunication *network = new NetCommunication;
    QString armVersion = "";
    QString armUrl = "";

    QString json_str = network->PackageJson_version(CABINETNO);
    if (network->PostHttp("version", json_str, 2))
    {
        if (UnpackVersion(network->ServerReply, &armVersion, &armUrl))
        {
            if(armVersion != VERSION);
            {
                QProcess *proe = new QProcess;
                proe->startDetached("./untitled4");
                this->deleteLater();
            }
        }
    }
}

bool Initprocess::UnpackVersion(QJsonDocument jd , QString *armVersion, QString *armUrl)
{
    QJsonObject analyze_Z = jd.object();
    QJsonValue s_json[3] = {0};
    bool s_success = false;

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    if (s_success)
    {
        s_json[0] = analyze_Z["armVersion"].toString();
        *armVersion = s_json[0].toString();

        s_json[1] = analyze_Z["armDownload"].toString();
        *armUrl = s_json[1].toString();
    }
    else
        return false;

    return true;
}
