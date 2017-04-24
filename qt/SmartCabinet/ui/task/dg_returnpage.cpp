#include "dg_returnpage.h"
#include "ui_dg_returnpage.h"
#include "common/globalvariable.h"
#include "ui/exec/dg_execreturn.h"
#include <QMessageBox>

Dg_ReturnPage::Dg_ReturnPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dg_ReturnPage)
{
    ui->setupUi(this);
    commonModel = new CommonModel(this);
    updateSQL = new UpdateSQL(this);
#ifndef TEST
    this->showFullScreen();
#endif
//      showMaximized();
//      setWindowFlags(Qt::FramelessWindowHint);
    DownLoad_TaskList();
    ShowGUI();
}

Dg_ReturnPage::~Dg_ReturnPage()
{
    delete ui;
}

void Dg_ReturnPage::on_pB_Exec_clicked()
{
    if (WriteSelected2SQL(ui->TW_Exec, "T_AgentiaReturnExecute"))
    {
        ShowExecuteWindow();
    }
}

void Dg_ReturnPage::on_CB_return_clicked()
{
    this->deleteLater();
}

void Dg_ReturnPage::DownLoad_TaskList()
{
    QSqlQuery query;
    query.exec(QString("DELETE from T_AgentiaReturnSave"));
    query.exec(QString("DELETE from T_AgentiaReturnExecute"));

    if (updateSQL->GetWaitReturnList(&userId))
    {
        ;
    }
    else
    {

        //后期加上需要将人员信息也作为删选的条件
        query.exec(QString("insert into T_AgentiaReturnSave (id,agentiaName,judgeAttitude,userDisplayName,\
                           bottleCapacity,dose,drawerNo,positionNo,expireDate,attribute,\
                           agentiaId,positionId,drawerId,agentiaStatus,userId) \
                            select * from T_AgentiaSaving where judgeAttitude='借出'"));

    }
}

//T_AgentiaReturnSave 归还——存储表
//T_AgentiaReturnExecute 归还——执行表

void Dg_ReturnPage::ShowGUI()
{
    //行选
    ui->TW_Exec->setSelectionBehavior(QAbstractItemView::SelectRows);
    //多行选中
    ui->TW_Exec->setSelectionMode(QAbstractItemView::MultiSelection);
    //禁止编辑
    ui->TW_Exec->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //取消表头焦点
    ui->TW_Exec->setFocusPolicy(Qt::NoFocus);
    //
    QHeaderView *Exec_header = ui->TW_Exec->verticalHeader();
    Exec_header->setHidden(true);

    QStringList hearders;
    hearders <<"编号" << "试剂名"<<"试剂状态"<<"借用者"<<"额定容量"<<"剩余容积"\
             <<"抽屉号"<<"位置号"<<" 失效日期"<<"试剂类别"<<"试剂ID"<<"位置ID"\
             <<"抽屉ID"<<"试剂状态"<<"借用者ID";

    commonModel->LoadSQL(ui->TW_Exec, "T_AgentiaReturnSave", hearders);

    ui->TW_Exec->sortItems(0, Qt::AscendingOrder);//升序
}

void Dg_ReturnPage::ShowExecuteWindow()
{
    Dg_ExecReturn *execReturn = new Dg_ExecReturn(this);
    execReturn->show();
    this->hide();
    resetSignal = HALTQUITPROCESS;//停止自动退出功能
    connect(execReturn, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

bool Dg_ReturnPage::WriteSelected2SQL(QTableWidget *source, QString table_name)
{
    QList<QTableWidgetItem *> array = source->selectedItems();
    int count = array.count();
    int old = -1;
    int current = -1;
    int nowCount = 0;
    QMap<int, QString> map;
    map.clear();

    for (int i = 0; i < count; i++)
    {
        int row = source->row(array.at(i));
        QTableWidgetItem *item = array.at(i);
        QString name = item->text();

        current = row;

        if (current != old)
        {
            for (int i = 0 ; i < map.size(); i++)
            {
                qDebug() <<map[i];
            }
            WriteSql(map, table_name);
            old = current;
            nowCount = 0;
            map.insert(nowCount, name);
        }
        else
        {
            nowCount++;
            map.insert(nowCount, name);

        }

        if (i == (count - 1))//将选中的最后一行输出
        {
            WriteSql(map, table_name);
        }
    }

    if (count == 0)
    {
        return false;
    }

    return true;
}

void Dg_ReturnPage::WriteSql(QMap<int, QString> map, QString name)
{
    if (!map.isEmpty())
    {
        QSqlQuery query;
        query.prepare(QString("select * from %1").arg(name));
        query.exec();
        query.last();
        qDebug()<<query.last();

        query.prepare(QString("insert into %1 (id,agentiaName,judgeAttitude,userDisplayName,\
                      bottleCapacity,dose,drawerNo,positionNo,expireDate,attribute,\
                      agentiaId,positionId,drawerId,agentiaStatus,userId) values (?,?,?,?,?,?,?\
                                                                                 ,?,?,?,?,?,?,?,?)").arg(name));

       query.addBindValue(map[0]);
       query.addBindValue(map[1]);
       query.addBindValue(map[2]);
       query.addBindValue(map[3]);
       query.addBindValue(map[4]);
       query.addBindValue(map[5]);
       query.addBindValue(map[6]);
       query.addBindValue(map[7]);
       query.addBindValue(map[8]);
       query.addBindValue(map[9]);
       query.addBindValue(map[10]);
       query.addBindValue(map[11]);
       query.addBindValue(map[12]);
       query.addBindValue(map[13]);
       query.addBindValue(map[14]);


        query.exec();
    }
}

