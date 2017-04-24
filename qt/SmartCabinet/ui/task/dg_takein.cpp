#include "dg_takein.h"
#include "ui_dg_takein.h"
#include "common/updatesql.h"
#include "common/globalvariable.h"
#include "common/commonmodel.h"
#include "ui/exec/dg_exectakein.h"
#include "ui/common/dg_requestposition.h"
#include <QSqlRecord>
#include <QMessageBox>

Dg_TakeIn::Dg_TakeIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dg_TakeIn)
{
    ui->setupUi(this);
    commonModel = new CommonModel(this);
    updateSQL = new UpdateSQL(this);
    selectRow = 0;

#ifndef TEST
    this->showFullScreen();
#endif
//      showMaximized();
//      setWindowFlags(Qt::FramelessWindowHint);
    DownLoad_TaskList();
    SetTitle("入柜界面");
    ShowUI();
    ShowSQL();
}

Dg_TakeIn::~Dg_TakeIn()
{
    delete ui;
}

void Dg_TakeIn::on_CB_return_clicked()
{
    this->deleteLater();
}

//执行按钮
//T_PutInExec  执行列表
void Dg_TakeIn::on_pB_exec_clicked()
{
    if (ui->TW_Exec->rowCount() != 0)//执行框不为空
    {
        if (GetContent(ui->TW_Exec, "T_PutInExec"))//将数据写入执行表单
        {
            ShowExecuteWindow(); //显示执行界面关闭本界面
        }
    }
}

//申请
void Dg_TakeIn::on_pB_delete_clicked()
{
    //删除待选框中试剂
    commonModel->Del(ui->TW_Exec);
    //根据编号排序
    SortAscend();
}
//删除按钮
void Dg_TakeIn::on_pB_apply_clicked()
{
    QList<QTableWidgetItem *> array = ui->TW_Sea->selectedItems();
    int count = array.count();
    int be = -1;

    for (int i = 0; i < count; i++)
    {
        int row = ui->TW_Sea->row(array.at(i));
        if (be != row)
        {

            Dg_RequestPosition *requestPage = new Dg_RequestPosition(this);
            QString agentiaName = ui->TW_Sea->item(row, 1)->text();
            requestPage->ShowAgentiaInfo(agentiaName);
            connect(requestPage, SIGNAL(ReplyNeedInfo(int,int,int,QString,QString,QString)), \
                    this, SLOT(AcceptPositionInfo(int,int,int,QString,QString,QString)));
            selectRow = row;

            requestPage->exec();
            break;
        }
    }
}

//写入表格
void Dg_TakeIn::AcceptPositionInfo(int positionNo, int positionId,\
                                   int drawer, QString dataTime, QString bottle,\
                                   QString dose)
{
    QString agentiaName = ui->TW_Sea->item(selectRow, 1)->text();
    QString judge = "入柜";
    QString drawerNo = QString::number(drawer);
    QString TpositionNo = QString::number(positionNo);
    QString bottleCapacity = bottle;
    QString Tdose = dose;
    QString expireDate = dataTime;
    QString agentiaTypeId = ui->TW_Sea->item(selectRow, 7)->text();
    QString TpositionId = QString::number(positionId);

    int rowCount = ui->TW_Exec->rowCount();
    ui->TW_Exec->setRowCount(rowCount + 1);

    QTableWidgetItem *test = new QTableWidgetItem();//插入整型
    int integer = ui->TW_Exec->rowCount();
    qDebug()<<integer;
    test->setData(Qt::DisplayRole, integer);
    ui->TW_Exec->setItem(rowCount, 0,test);

    ui->TW_Exec->setItem(rowCount, 1, new QTableWidgetItem(agentiaName));
    ui->TW_Exec->setItem(rowCount, 2, new QTableWidgetItem(judge));
    ui->TW_Exec->setItem(rowCount, 3, new QTableWidgetItem(drawerNo));
    ui->TW_Exec->setItem(rowCount, 4, new QTableWidgetItem(TpositionNo));
    ui->TW_Exec->setItem(rowCount, 5, new QTableWidgetItem(bottleCapacity));
    ui->TW_Exec->setItem(rowCount, 6, new QTableWidgetItem(Tdose));
    ui->TW_Exec->setItem(rowCount, 7, new QTableWidgetItem(expireDate));
    ui->TW_Exec->setItem(rowCount, 8, new QTableWidgetItem(agentiaTypeId));
    ui->TW_Exec->setItem(rowCount, 9, new QTableWidgetItem(TpositionId));

}

void Dg_TakeIn::ShowPage()
{
    this->show();
}

void Dg_TakeIn::on_pB_search_clicked()
{
    QString searchGoal = "";

    searchGoal = ui->LE_search->text();
    //输入为空显示所有信息
    //不为空显示相关信息
    commonModel->ScreenInfo(ui->TW_Sea, searchGoal);//信息检索
}


void Dg_TakeIn::ShowUI()
{
    //行选
    ui->TW_Exec->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TW_Sea->setSelectionBehavior(QAbstractItemView::SelectRows);
    //多行选中
    ui->TW_Exec->setSelectionMode(QAbstractItemView::MultiSelection);
    //单行选中
    ui->TW_Sea->setSelectionMode(QAbstractItemView::SingleSelection);
    //禁止编辑
    ui->TW_Exec->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TW_Sea->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //取消表头焦点
    ui->TW_Exec->setFocusPolicy(Qt::NoFocus);
    ui->TW_Sea->setFocusPolicy(Qt::NoFocus);
    //
    QHeaderView *Exec_header = ui->TW_Exec->verticalHeader();
    QHeaderView *Sea_header = ui->TW_Sea->verticalHeader();
    Exec_header->setHidden(true);
    Sea_header->setHidden(true);
}

//T_AgentiaTypeList
//T_PutInExec
void Dg_TakeIn::ShowSQL()
{
    QStringList hearders_T;
    QStringList hearders_E;
    hearders_T <<"编号" << "全称"<<"简称"<<"化学式"\
             <<"规格"<<"厂家"<<"类别"<<"试剂种类ID"\
             <<"产品型号";

    hearders_E <<"编号" << "试剂名"<<"状态"<<"抽屉号"\
              <<"位置号"<<"额定容积"<<"实际容积"<<"失效日期"\
              <<"试剂种类ID"<<"位置ID";

    commonModel->LoadSQL(ui->TW_Exec, "T_PutInExec", hearders_E);
    commonModel->LoadSQL(ui->TW_Sea, "T_AgentiaTypeList", hearders_T);
}



//升序
void Dg_TakeIn::SortAscend()
{
    ui->TW_Exec->sortItems(0, Qt::AscendingOrder);
    ui->TW_Sea->sortItems(0, Qt::AscendingOrder);
}

void Dg_TakeIn::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton )
    {
        ui->TW_Exec->selectionModel()->clearSelection();
        ui->TW_Sea->selectionModel()->clearSelection();
        resetSignal = RESETTIMER;
    }
}

bool Dg_TakeIn::GetContent(QTableWidget *source, QString name)
{
    int rowCount = source->rowCount();
    int columnCount = source->columnCount();
    int WriteSQL_BeforeRowCount = 0;
    int WriteSQL_LaterRowCount = 0;
    QString content = "";
    QTableWidgetItem *item;
    QSqlQuery qry;

    qry.exec(QString("DELETE from %1").arg(name));//清空执行表

    for (int i = 0; i < rowCount; i++)
    {
        QMap <int, QString> map;

        for (int j = 0; j < columnCount; j++)
        {
            item = source->item(i,j);
            content = item->text();
            map.insert(j, content);

        }
        Write_Sql(map, name);
    }

    qry.exec(QString("select * from %1").arg(name));
    qry.last();
    WriteSQL_LaterRowCount = qry.at() + 1;

    //执行前后行数未变
    if (WriteSQL_BeforeRowCount == WriteSQL_LaterRowCount || WriteSQL_LaterRowCount == -1)
    {
        QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("写入 %1 失败")\
                             .arg(name));
        return false;
    }

    return true;
}

//T_PutInExec  执行列表
void Dg_TakeIn::Write_Sql(QMap<int, QString> map, QString &name)
{
    QSqlQuery query;
    query.prepare(QString("select * from %1").arg(name));
    query.exec();
    query.last();
    qDebug()<<query.last();

    query.prepare(QString("insert into %1 (id,agentiaName,judgeAttitude,drawerNo,\
                  positionNo,bottleCapacity,dose,expireDate,agentiaTypeId,\
                          positionId) values (?,?,?,?,?,?,?,?,?,?)").arg(name));

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

   query.exec();
}

void Dg_TakeIn::ShowExecuteWindow()
{
    Dg_ExecTakeIn *execTakeIn = new Dg_ExecTakeIn(this);
    execTakeIn->show();
    this->hide();
    resetSignal = HALTQUITPROCESS;//停止自动退出功能

    connect(execTakeIn, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

void Dg_TakeIn::DownLoad_TaskList()
{
    QSqlQuery query;
    query.exec(QString("DELETE from T_PutInExec"));
    updateSQL->GetTaskList(&userId, 0);
    updateSQL->GetAgentiaType();
}

void Dg_TakeIn::SetTitle(QString name)
{
    ui->LB_title->setText(QStringLiteral("%1").arg(name));
}
