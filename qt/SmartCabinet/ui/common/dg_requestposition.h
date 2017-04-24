#ifndef DG_REQUESTPOSITION_H
#define DG_REQUESTPOSITION_H

#include <QDialog>
#include "network/netcommunication.h"

namespace Ui {
class Dg_RequestPosition;
}

class Dg_RequestPosition : public QDialog
{
    Q_OBJECT

public:
    explicit Dg_RequestPosition(QWidget *parent = 0);
    ~Dg_RequestPosition();

    void ShowAgentiaInfo(QString agentiaName);

signals:
    //返回server分配的位置信息
    void ReplyNeedInfo(int positionNo, int positionId,\
                       int drawer, QString dataTime, QString bottle,\
                       QString dose);
private slots:

    void on_pB_back_clicked();

    void on_pB_apply_clicked();

private:
    Ui::Dg_RequestPosition *ui;
    NetCommunication *apply_netcommunication;

    int positionNo;
    int positionId;
    bool UnpackPosition(QJsonDocument jd);
};

#endif // DG_REQUESTPOSITION_H
