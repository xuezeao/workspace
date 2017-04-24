#include "showallinfo.h"
#include "ui_showallinfo.h"
#include <qsqlquery.h>
#include <QDebug>
#include <QDesktopWidget>


ShowAllInfo::ShowAllInfo(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::ShowAllInfo)
{
    ui->setupUi(this);
    //    this->showFullScreen();
//        showMaximized();
//        setWindowFlags(Qt::FramelessWindowHint);

//    move((QApplication::desktop()->width()-this->width())/2,(QApplication::desktop()->height()-this->height())/2);//居中

    T_model_show = new QSqlTableModel;
    delayTime = new DelayTime;
    ui->tableView_showInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

ShowAllInfo::~ShowAllInfo()
{
    delete ui;
}
/*****************************************/
void ShowAllInfo::on_pBt_close_clicked()
{
    /***************清空表格************/
    QSqlQuery query;
    query.exec(QString("DELETE from %1").arg(T_name));


    /********************************/
    emit upStatus();
    delayTime->waitTaskInfo(100);
    this->close();
}

/******************************************/
void ShowAllInfo::saveNotPostInfo(int order)
{
    int getC_agentiaId;
    int getC_positionId;
    QString getC_dose;
    QString getC_status;
    QSqlQuery query;

    query.exec(QString("select * from T_UserInfo"));
    query.seek(0);
    int getC_userId = query.value(0).toInt();//获取用户ID

    query.exec(QString("select * from T_WaitPostInfo"));
    query.last();
    int getC_all = query.at()+1;//获取数据表格row

    query.exec(QString("select * from %1").arg(T_name));
    query.last();
    int allLong = query.at()+1;


    if (order == 1)
    {
        for (int i = 0; i<allLong; i++)
        {
            query.seek(i);
            getC_status = query.value(13).toString();//状态
            if (getC_status == "正确操作" || getC_status == "上传失败")
            {
                getC_agentiaId = query.value(11).toInt();//试剂ID
                getC_positionId = query.value(12).toInt();//位置ID
                getC_dose       = query.value(4).toString();//dose

                query.prepare("insert into T_WaitPostInfo (id,userId,agentiaId,\
                              positionId,dose,judgeAttitude) values (?,?,?,?,?,?)");
                getC_all++;

                query.addBindValue(getC_all);
                query.addBindValue(getC_userId);
                query.addBindValue(getC_agentiaId);
                query.addBindValue(getC_positionId);
                query.addBindValue("0");
                query.addBindValue("");
                query.exec();
            }
        }
    }
    else if (order == 2)
    {
        for (int i = 0; i<allLong; i++)
        {
            query.seek(i);
            getC_status = query.value(13).toString();//状态
            if(getC_status == "正确操作" || getC_status == "上传失败")
            {
                getC_agentiaId = query.value(11).toInt();//试剂ID
                getC_positionId = query.value(12).toInt();//位置ID
                getC_dose       = query.value(4).toString();//dose

                query.prepare("insert into T_WaitPostInfo (id,userId,agentiaId,\
                              positionId,dose,judgeAttitude) values (?,?,?,?,?,?)");
                getC_all++;

                query.addBindValue(getC_all);
                query.addBindValue(getC_userId);
                query.addBindValue(getC_agentiaId);
                query.addBindValue(getC_positionId);
                query.addBindValue("0");
                query.addBindValue("");
                query.exec();
            }
        }
    }
    else if ((order == 4) ||(order == 6))
    {
        for (int i = 0; i<allLong; i++)
        {
            query.seek(i);
            getC_status = query.value(13).toString();//状态

            if ((getC_status == "正确操作") || (getC_status == "上传失败"))
            {
                getC_agentiaId = query.value(11).toInt();//试剂ID
                getC_positionId = query.value(12).toInt();//位置ID
                getC_dose       = query.value(4).toString();//dose

                query.prepare("insert into T_WaitPostInfo (id,userId,agentiaId,\
                              positionId,dose,judgeAttitude) values (?,?,?,?,?,?)");
                getC_all++;

                query.addBindValue(getC_all);
                query.addBindValue(getC_userId);
                query.addBindValue(getC_agentiaId);
                query.addBindValue(getC_positionId);
                query.addBindValue("0");
                query.addBindValue("");
                query.exec();
            }
        }
    }
}

void ShowAllInfo::InitVariable(int order)
{
    switch (order) {
    case 1:{
        T_name = "T_Task_PutIn";
        break;
    }
    case 2:{
        T_name = "T_AgentiaWaitExecute";
        break;
    }
    case 4:{
        T_name = "T_AgentiaReplace";
        break;
    }
    case 6:{
        T_name = "T_AgentiaCheck";
    }
    default:
        break;
    }

    saveNotPostInfo(order);
}

void ShowAllInfo::showInfo(int order)//0：入柜 1：还 2:替换 3:点验
{
    InitVariable(order);

    if ((order == 4) || (order == 6)) // 替换  点验
    {

        T_model_show->setTable(QString("%1").arg(T_name));
        T_model_show->select();
        T_model_show->setHeaderData(3,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_show->setHeaderData(6,Qt::Horizontal,QObject::tr("试剂容量"));
        T_model_show->setHeaderData(8,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_show->setHeaderData(9,Qt::Horizontal,QObject::tr("位置"));
        T_model_show->setHeaderData(13,Qt::Horizontal,QObject::tr("状态"));

        T_model_show->setEditStrategy(QSqlTableModel::OnManualSubmit);

        ui->tableView_showInfo->setModel(T_model_show);

        ui->tableView_showInfo->setColumnHidden(0,true);
        ui->tableView_showInfo->setColumnHidden(1,true);
        ui->tableView_showInfo->setColumnHidden(2,true);
        ui->tableView_showInfo->setColumnHidden(4,true);
        ui->tableView_showInfo->setColumnHidden(5,true);
        ui->tableView_showInfo->setColumnHidden(7,true);
        ui->tableView_showInfo->setColumnHidden(10,true);
        ui->tableView_showInfo->setColumnHidden(11,true);
        ui->tableView_showInfo->setColumnHidden(12,true);
    }
    else if (order == 2) //还
    {

        T_model_show->setTable(QString("%1").arg(T_name));
        T_model_show->select();

        T_model_show->setHeaderData(2,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_show->setHeaderData(4,Qt::Horizontal,QObject::tr("试剂容量"));
        T_model_show->setHeaderData(9,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_show->setHeaderData(10,Qt::Horizontal,QObject::tr("位置"));
        T_model_show->setHeaderData(14,Qt::Horizontal,QObject::tr("状态"));

        T_model_show->setEditStrategy(QSqlTableModel::OnManualSubmit);

        ui->tableView_showInfo->setModel(T_model_show);

        ui->tableView_showInfo->setColumnHidden(0,true);
        ui->tableView_showInfo->setColumnHidden(1,true);
        ui->tableView_showInfo->setColumnHidden(3,true);
        ui->tableView_showInfo->setColumnHidden(5,true);
        ui->tableView_showInfo->setColumnHidden(6,true);
        ui->tableView_showInfo->setColumnHidden(7,true);
        ui->tableView_showInfo->setColumnHidden(8,true);
        ui->tableView_showInfo->setColumnHidden(11,true);
        ui->tableView_showInfo->setColumnHidden(12,true);
        ui->tableView_showInfo->setColumnHidden(13,true);

    }
    else if (order == 1) // 入柜
    {    
        T_model_show->setTable(QString("%1").arg(T_name));
        T_model_show->select();
        T_model_show->setHeaderData(3,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_show->setHeaderData(4,Qt::Horizontal,QObject::tr("状态"));
        T_model_show->setHeaderData(6,Qt::Horizontal,QObject::tr("试剂容量"));


        T_model_show->setEditStrategy(QSqlTableModel::OnManualSubmit);

        ui->tableView_showInfo->setModel(T_model_show);

        ui->tableView_showInfo->setColumnHidden(0,true);
        ui->tableView_showInfo->setColumnHidden(1,true);
        ui->tableView_showInfo->setColumnHidden(2,true);
        ui->tableView_showInfo->setColumnHidden(5,true);
        ui->tableView_showInfo->setColumnHidden(7,true);
        ui->tableView_showInfo->setColumnHidden(8,true);
        ui->tableView_showInfo->setColumnHidden(9,true);
        ui->tableView_showInfo->setColumnHidden(10,true);
        ui->tableView_showInfo->setColumnHidden(11,true);
        ui->tableView_showInfo->setColumnHidden(12,true);
        ui->tableView_showInfo->setColumnHidden(13,true);
        ui->tableView_showInfo->setColumnHidden(14,true);
        ui->tableView_showInfo->setColumnHidden(15,true);
        ui->tableView_showInfo->setColumnHidden(16,true);
        ui->tableView_showInfo->setColumnHidden(17,true);
        ui->tableView_showInfo->setColumnHidden(18,true);
    }

    /*************************************/


}







