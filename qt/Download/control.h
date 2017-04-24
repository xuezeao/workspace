#ifndef CONTROL_H
#define CONTROL_H

#include "netcommunication.h"
#include <QObject>
#include <QTextBrowser>
#include <QWidget>

class Control : public QWidget
{
    Q_OBJECT
public:
    explicit Control(QWidget *parent = 0);

signals:

public slots:
    void GetVersion();

private:
    NetCommunication *netcommunication;
    QTextBrowser *setBrowser;
    QString info_output;

    QString armVersion;
    QString armUrl;


    void Download_shell_del_down(QString name);
    bool UnpackVersion(QJsonDocument jd, QString *armVersion, QString *armUrl);
    void ShowExecStep(QString info);
    void Decompression(QString flie);
    void WaitTaskInfo(int tim);
    void InstallExe(QString exeName);
    const char* Translate(QString info);
    bool StarSQL();
};

#endif // CONTROL_H
