#include "control.h"
#include "globalvariable.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegExp>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTimer>

Control::Control(QWidget *parent) : QWidget(parent)
{
    netcommunication = new NetCommunication;
    setBrowser = new QTextBrowser;
    info_output = "";
    armVersion = "";
    armUrl = "";

    QHBoxLayout *hBox = new QHBoxLayout;
    hBox->addWidget(setBrowser);
    setLayout(hBox);

    this->show();

    QTimer *timer = new QTimer;
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(GetVersion()));
//    this->showFullScreen();
}

void Control::GetVersion()
{
    QString downloadFileName = "";
    ShowExecStep("获取下载地址....");

    QString json_str = netcommunication->PackageJson_version(CABINETNO);
    if (netcommunication->PostHttp("version", json_str, 2))
    {
        if (UnpackVersion(netcommunication->ServerReply, &armVersion, &armUrl))
        {
            qDebug()<< armVersion <<"   "<<armUrl;
            QString pattern("/download/(.*)");
            QRegExp re(pattern);
            int pos = armUrl.indexOf(re);
            if (pos >= 0)
            {

                downloadFileName = re.cap(1);

                ShowExecStep("下载地址获取成功，正在下载...");
                WaitTaskInfo(5);
                Download_shell_del_down(downloadFileName);

                ShowExecStep("正在解压...");
                WaitTaskInfo(5);
                Decompression(downloadFileName);
                ShowExecStep("解压成功");

                ShowExecStep("安装中请稍后...");
                WaitTaskInfo(5);
                InstallExe("untitled");
                ShowExecStep("安装完成");
                StarSQL();
            }


        }
        else
            ShowExecStep("获取失败，关闭下载程序");
    }
    else
        ShowExecStep("获取失败,关闭下载程序");

    this->deleteLater();
}

//    system("wget -P/home/x/SmartCabinet -c http://121.43.159.215:3000/download/arm_test.tar"); //download url demo
void Control::Download_shell_del_down(QString name)
{
    QString deleteAlreadyExistFile = QString("rm -rf %1/%2").arg(localFileName).arg(name);
    system(Translate(deleteAlreadyExistFile));


    QString url_download = QString("wget -P%1 -c %2%3")\
            .arg(localFileName).arg(serverIp).arg(armUrl);
    system(Translate(url_download));
}

const char* Control::Translate(QString info)
{
    QByteArray translateInfo = info.toLatin1();
    const char *c_str = translateInfo.data();
    return c_str;
}

void Control::Decompression(QString file)
{
    QString deleteNeedUpgradeFile = QString("rm -rf %1/%2").arg(localFileName).arg(goalFile);
    system(Translate(deleteNeedUpgradeFile));

    QString unpackageFile = QString("tar -xf %1/%2 -C %1")
            .arg(localFileName).arg(file);
    system(Translate(unpackageFile));

    QString deleteAlreadyExistFile = QString("rm -rf %1/%2").arg(localFileName).arg(file);
    system(Translate(deleteAlreadyExistFile));
}

void Control::InstallExe(QString exeName)
{
    QString programName = QString("%1/%2/%3").arg(localFileName).arg(goalFile).arg(exeName);
    QProcess *proe = new QProcess;
    proe->start(Translate(programName));
}

bool Control::UnpackVersion(QJsonDocument jd , QString *armVersion, QString *armUrl)
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

void Control::ShowExecStep(QString info)
{
    QString newLine = "\n";
    info_output += newLine + info;
    setBrowser->setText(info_output);
}

void Control::WaitTaskInfo(int tim)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<tim)
    {
        QCoreApplication::processEvents();
    }
}

bool Control::StarSQL()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("SmartCabinet.db");
    if(!db.open())
        return false;

    QSqlQuery query;

    qDebug()<<query.exec(QString("select * from T_Version"));
    query.last();
    qDebug()<<query.value(0).toString();
    qDebug()<<query.value(1).toString();

    qDebug()<<query.exec(QString("update T_Version set versionNo='%1'")
                           .arg(armVersion));

    return true;
}
