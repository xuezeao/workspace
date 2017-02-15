#include "agentiainfochange.h"
#include "ui_agentiainfochange.h"
#include <QRegExp>
#include <QValidator>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QDesktopWidget>

AgentiaInfoChange::AgentiaInfoChange(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::AgentiaInfoChange)
{
    ui->setupUi(this);
    //    this->showFullScreen();
//        showMaximized();
//        setWindowFlags(Qt::FramelessWindowHint);

    //    move((QApplication::desktop()->width()-this->width())/2,(QApplication::desktop()->height()-this->height())/2);//居中

    QRegExp regx_bottleCapacity("[0-9]+$");//"[a-zA-Z0-9]+$" 长度7纯数字输入3.3格式
    QValidator *validator_bottleCapacity = new QRegExpValidator(regx_bottleCapacity,ui->lineEdit_dose);
    ui->lineEdit_dose->setValidator(validator_bottleCapacity);

    QRegExp regx_dose("[0-9]+$");//"[a-zA-Z0-9]+$" 长度7纯数字输入3.3格式
    QValidator *validator_dose = new QRegExpValidator(regx_dose,ui->lineEdit_dose);
    ui->lineEdit_dose->setValidator(validator_dose);




    ui->dateEdit_expireDate->setCalendarPopup(true);
    ui->dateEdit_expireDate->setDisplayFormat("yyyy.M.d");//设置日期格式
    ui->dateEdit_expireDate->setCalendarPopup(true);
    ui->dateEdit_expireDate->setDate(QDate::currentDate());
}

void AgentiaInfoChange::closeWindow_operate()
{
    addInfo_To_sheet(AInfo_sheetOrder);
    emit upSheet_PutIn();
    this->close();
}

void AgentiaInfoChange::sendUP_Info(int status ,int OK_Push, int sheetOrder)//选择显示的模式 //0 集体修改；1 单个修改
{

    AInfo_OK_Push = OK_Push;
    AInfo_sheetOrder = 0;

    if(status == 1)
    {
        ui->pBt_nextOperate->setText("完成");
        ui->pBt_previousOperate->setText("上一条");
        ui->pBt_previousOperate->setEnabled(false);
        AInfo_sheetOrder = sheetOrder;

        changeShowInfo(sheetOrder);
    }
    else if(status == 0)
    {
        ui->pBt_nextOperate->setText("下一条");
        ui->pBt_previousOperate->setText("完成");
        ui->pBt_previousOperate->setEnabled(true);

        changeShowInfo(0);//从第一条开始执行
    }

    /*************************************/

}

AgentiaInfoChange::~AgentiaInfoChange()
{
    delete ui;
}

void AgentiaInfoChange::on_pBt_returnPrevious_clicked()
{
    closeWindow_operate();
}

void AgentiaInfoChange::on_pBt_nextOperate_clicked()
{

    QSqlQuery query;
    query.exec(QString("select * from %1").arg(changeTabel));
    query.last();//定位到最后一条
    int rowNum = query.at();//获取 rowNum

    if((AInfo_OK_Push == 1) || (AInfo_sheetOrder >= rowNum && AInfo_OK_Push == 0))
    {
        closeWindow_operate();
    }
    else
    {
        if(AInfo_sheetOrder < rowNum)
        {
            addInfo_To_sheet(AInfo_sheetOrder);
            AInfo_sheetOrder++;
            changeShowInfo(AInfo_sheetOrder);
            ui->pBt_previousOperate->setText("上一步");
            AInfo_OK_Push = 0;
        }
        else if(AInfo_sheetOrder >= rowNum)
        {
            ui->pBt_nextOperate->setText("完成");
            AInfo_OK_Push = 1;
            AInfo_sheetOrder = rowNum;

        }
    }

    qDebug()<<"AInfo_Sheet: "<<AInfo_sheetOrder << "指明修改的信息";
}

void AgentiaInfoChange::on_pBt_previousOperate_clicked()
{

    if((AInfo_OK_Push == 2) || (AInfo_sheetOrder <= 0 && AInfo_OK_Push == 0))
    {
        closeWindow_operate();
    }
    else
    {
        if(AInfo_sheetOrder > 0)
        {
            addInfo_To_sheet(AInfo_sheetOrder);
            AInfo_sheetOrder--;
            changeShowInfo(AInfo_sheetOrder);
            ui->pBt_nextOperate->setText("下一步");
            AInfo_OK_Push = 0;
        }
        else if(AInfo_sheetOrder <= 0)//为0的时候切换为完成功能
        {
            ui->pBt_previousOperate->setText("完成");
            AInfo_OK_Push = 2;
        }
    }

    qDebug()<<"AInfo_Sheet: "<<AInfo_sheetOrder\
              <<"OK: "<< AInfo_OK_Push;
}


void AgentiaInfoChange::addInfo_To_sheet(int i)
{
    QString getC_bottledose = ui->CMB_bottleCapacity->currentText();
    QString getC_bottleCapacity = ui->lineEdit_bottleCapacity->text();
    QString getC_doseUint = ui->CMB_dose->currentText();
    QString getC_dose = ui->lineEdit_dose->text();

    getC_dose += getC_doseUint;
    getC_bottleCapacity += getC_bottledose;

    QString getC_expireDate = ui->dateEdit_expireDate->text();
    QString getC_drawerSize = ui->comboBox_drawerSize->currentText();

    QSqlQuery query;

    if(modelChoice == 1)//入柜
    {
        query.exec(QString("update T_Task_PutIn set bottleCapacity='%1',dose='%2',\
                           drawerSize='%3',expireDate='%4' where rowid=%5")
                   .arg(getC_bottleCapacity).arg(getC_dose).arg(getC_drawerSize).arg(getC_expireDate).arg(i+1));//rowid 从1开始
    }
    else if(modelChoice == 0)//替换
    {
        QString getC_newDose = ui->lineEdit_newdose->text();
        QString getC_newDoseUint = ui->CMB_newdose->currentText();

        getC_newDose += getC_newDoseUint;

        query.exec(QString("update %4 set newdose='%1',expireDate='%2' where rowid=%3")
                   .arg(getC_newDose).arg(getC_expireDate).arg(i+1).arg(changeTabel));//rowid 从1开始

        ui->lineEdit_newdose->clear();
    }


}

void AgentiaInfoChange::changeShowInfo(int i)
{
    QSqlQuery query;
    query.exec(QString("select * from %1").arg(changeTabel));
    query.seek(i);

    QString getC_AName = query.value(3).toString();//试剂名
    int getC_Attribute = query.value(10).toInt();//试剂类别

    if(modelChoice == 1)
    {
        int getC_ID = query.value(11).toInt();//试剂ID
        QString getC_LFormula = query.value(12).toString();//化学式
        QString getC_Specification = query.value(13).toString();//详细介绍
        QString getC_Factory = query.value(14).toString();//厂商
        QString getC_ItemNo = query.value(15).toString();//项目编号
        QString getC_drawerSize = query.value(7).toString();//规格

        ui->label_chemicalFormula->setText(getC_LFormula);
        ui->textBrowser_specification->setText(getC_Specification);
        ui->label_factory->setText(getC_Factory);
        ui->label_itemNo->setText(getC_ItemNo);
        ui->comboBox_drawerSize->setCurrentText(getC_drawerSize);
        ui->label_agentiaID->setText(QString("%1").arg(getC_ID));
    }

    QString getC_bottleCapacity = query.value(5).toString();//额定容量
    QString getC_dose = query.value(6).toString();//实际容量
    QString getC_expireDate = query.value(8).toString();//失效日期

    getC_dose.replace("ml","");
    getC_dose.replace("g","");
    getC_bottleCapacity.replace("ml","");
    getC_bottleCapacity.replace("g","");

    ui->lineEdit_bottleCapacity->setText(getC_bottleCapacity);
    ui->lineEdit_dose->setText(getC_dose);

    dateUp(getC_expireDate);

    ui->label_agentiaName->setText(getC_AName);
    ui->label_attribute->setText(QString("%1").arg(getC_Attribute));

}

void AgentiaInfoChange::dateUp(QString dateTime)//更新日期显示
{
    int acountDrop = 0;
    QString year_W = "";
    QString month_W = "";
    QString day_W = "";

    for(int i = 0 ; i < dateTime.length() ; i++)
    {
         if(dateTime[i] != '.')
         {
             if(acountDrop == 0)
             {
                 year_W.append(dateTime[i]);
             }
             else if(acountDrop == 1)
             {
                 month_W.append(dateTime[i]);
             }
             else if(acountDrop == 2)
             {
                 day_W.append(dateTime[i]);
             }
         }
         else
         {
             acountDrop++;
         }
    }

    QDate setDateTime = QDate(year_W.toInt(),month_W.toInt(),day_W.toInt());
    ui->dateEdit_expireDate->setDate(setDateTime);

}

void AgentiaInfoChange::receiverUpEnable(int status)//0:注销 1：恢复
{
    if(status == 0) // 替换
    {
        ui->lineEdit_dose->setEnabled(false);
        ui->lineEdit_bottleCapacity->setEnabled(false);
        ui->CMB_bottleCapacity->setEnabled(false);
        ui->CMB_dose->setEnabled(false);
        ui->comboBox_drawerSize->setEnabled(false);
        ui->lineEdit_newdose->show();
        ui->label_newdose->show();
        ui->CMB_newdose->show();
        ui->lineEdit_newdose->clear();

        modelChoice = 0;
        changeTabel = "T_AgentiaReplace";
    }
    else if(status == 1) //入柜
    {
        ui->lineEdit_dose->setEnabled(true);
        ui->lineEdit_bottleCapacity->setEnabled(true);
        ui->CMB_bottleCapacity->setEnabled(true);
        ui->CMB_dose->setEnabled(true);
        ui->comboBox_drawerSize->setEnabled(true);
        ui->label_newdose->hide();
        ui->lineEdit_newdose->hide();
        ui->CMB_newdose->hide();

        modelChoice = 1;
        changeTabel = "T_Task_PutIn";
    }
    else if(status == 2)//报废
    {
        ui->lineEdit_dose->setEnabled(false);
        ui->lineEdit_bottleCapacity->setEnabled(false);
        ui->CMB_bottleCapacity->setEnabled(false);
        ui->CMB_dose->setEnabled(false);
        ui->comboBox_drawerSize->setEnabled(false);
        ui->lineEdit_newdose->show();
        ui->label_newdose->show();
        ui->CMB_newdose->show();
        ui->lineEdit_newdose->clear();

        modelChoice = 0;
        changeTabel = "T_AgentiaCheck";
    }

}

