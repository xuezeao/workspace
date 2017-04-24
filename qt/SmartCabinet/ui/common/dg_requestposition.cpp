/*此页面用于申请试剂位置
 *如果分配成功则写入待执行试剂列表
 *分配失败则要求重新输入
 *
*/

#include "dg_requestposition.h"
#include "ui_dg_requestposition.h"
#include "common/globalvariable.h"
#include <QMessageBox>


Dg_RequestPosition::Dg_RequestPosition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dg_RequestPosition)
{
    ui->setupUi(this);
    apply_netcommunication = new NetCommunication(this);

    positionNo = 0;
    positionId = 0;

    ui->dateEdit->setCalendarPopup(true);
    ui->dateEdit->setDisplayFormat("yyyy.M.d");//设置日期格式
    ui->dateEdit->setCalendarPopup(true);
    ui->dateEdit->setDate(QDate::currentDate());

}

Dg_RequestPosition::~Dg_RequestPosition()
{
    apply_netcommunication->deleteLater();
    delete ui;
}



void Dg_RequestPosition::ShowAgentiaInfo(QString agentiaName)
{
    ui->TB_info->setText(QString("试剂名：%1").arg(agentiaName));
}

bool Dg_RequestPosition::UnpackPosition(QJsonDocument jd)
{
    QJsonObject analyze_Z = jd.object();
    QJsonValue s_json[3] = {0};
    bool s_success = false;

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    if (s_success)
    {
        s_json[0] = analyze_Z["positionNo"].toInt();
        positionNo = s_json[0].toInt();

        s_json[1] = analyze_Z["positionId"].toInt();
        positionId = s_json[1].toInt();
    }
    else
        return false;

    return true;
}

void Dg_RequestPosition::on_pB_back_clicked()
{
    this->deleteLater();
}



void Dg_RequestPosition::on_pB_apply_clicked()
{
    int drawerNo = ui->cB_drawer->currentText().toInt();
    QString json_str = apply_netcommunication->PackageJson_allocPosition(CABINETNO, drawerNo);
    if (apply_netcommunication->PostHttp("allocPosition", json_str, 2))
    {
        if (UnpackPosition(apply_netcommunication->ServerReply))
        {
            QString date = ui->dateEdit->text();
            QString dose_unit = ui->cB_dose->currentText();
            QString dose = ui->lE_dose->text();
            dose += dose_unit;
            QString bottle_unit = ui->cB_bottle->currentText();
            QString bottle = ui->lE_bottle->text();
            bottle += bottle_unit;
            int drawer = ui->cB_drawer->currentText().toInt();

            emit ReplyNeedInfo(positionNo, positionId, drawer, date, bottle, dose);
        }
        else
        {
            QMessageBox::warning(NULL, "Error", QStringLiteral("请重选抽屉"));
        }
    }
    else
    {
        QMessageBox::warning(NULL, "Error", QStringLiteral("网络异常，请稍后再试"));
    }
    this->deleteLater();
}
