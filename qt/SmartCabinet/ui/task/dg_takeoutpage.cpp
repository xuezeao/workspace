#include "dg_takeoutpage.h"
#include "ui_dg_takeoutpage.h"
#include "common/updatesql.h"
#include "common/globalvariable.h"
#include "common/commonmodel.h"
#include "ui/exec/dg_exectakeout.h"
#include <QSqlRecord>
#include <QMessageBox>
Dg_TakeOutPage::Dg_TakeOutPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dg_TakeOutPage)
{
    ui->setupUi(this);
    commonModel = new CommonModel(this);
    updateSQL = new UpdateSQL(this);

#ifndef TEST
    this->showFullScreen();
#endif
//      showMaximized();
//      setWindowFlags(Qt::FramelessWindowHint);
    table_exec = "T_AgentiaExecute";
    table_sea = "T_AgentiaSaving";

    DownLoad_TaskList();
    SetTitle("取界面");
    ShowUI();
    ShowSQL();
}

Dg_TakeOutPage::~Dg_TakeOutPage()
{
    delete ui;
}

void Dg_TakeOutPage::on_CB_return_clicked()
{
    this->deleteLater();
}

//执行按钮
//T_AgentiaExecute  执行列表
//T_AgentiaSaving 试剂列表
void Dg_TakeOutPage::on_pB_exec_clicked()
{
    if (ui->TW_Exec->rowCount() != 0)//执行框不为空
    {
        if (GetContent(ui->TW_Exec, "T_AgentiaExecute"))//将数据写入执行表单
        {
            ShowExecuteWindow(); //显示执行界面关闭本界面
        }
    }
}

//申请
void Dg_TakeOutPage::on_pB_delete_clicked()
{
    //将待选框试剂加入等待执行框
    Add(ui->TW_Sea, ui->TW_Exec);
    //删除待选框中试剂
    Del(ui->TW_Exec);
    //根据编号排序
    SortAscend();
}
//删除按钮
void Dg_TakeOutPage::on_pB_apply_clicked()
{
    //将待选框试剂加入等待执行框
    Add(ui->TW_Exec, ui->TW_Sea);
    //删除待选框中试剂
    Del(ui->TW_Sea);
    //根据编号排序
    SortAscend();
}

void Dg_TakeOutPage::on_pB_search_clicked()
{
    QString searchGoal = "";

    searchGoal = ui->LE_search->text();
    //输入为空显示所有信息
    //不为空显示相关信息
    commonModel->ScreenInfo(ui->TW_Sea, searchGoal);//信息检索
}


void Dg_TakeOutPage::ShowUI()
{
    //行选
    ui->TW_Exec->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TW_Sea->setSelectionBehavior(QAbstractItemView::SelectRows);
    //多行选中
    ui->TW_Exec->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->TW_Sea->setSelectionMode(QAbstractItemView::MultiSelection);
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

void Dg_TakeOutPage::ShowSQL()
{
    QStringList hearders;
    hearders <<"编号" << "试剂名"<<"试剂状态"<<"借用者"<<"额定容量"<<"剩余容积"\
             <<"抽屉号"<<"位置号"<<" 失效日期"<<"试剂类别"<<"试剂ID"<<"位置ID"\
             <<"抽屉ID"<<"试剂状态"<<"借用者ID";

    commonModel->LoadSQL(ui->TW_Exec, table_exec, hearders);
    commonModel->LoadSQL(ui->TW_Sea, table_sea, hearders);
}



//升序
void Dg_TakeOutPage::SortAscend()
{
    ui->TW_Exec->sortItems(0, Qt::AscendingOrder);
    ui->TW_Sea->sortItems(0, Qt::AscendingOrder);
}

void Dg_TakeOutPage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton )
    {
        ui->TW_Exec->selectionModel()->clearSelection();
        ui->TW_Sea->selectionModel()->clearSelection();
        resetSignal = RESETTIMER;
    }
}

bool Dg_TakeOutPage::GetContent(QTableWidget *source, QString name)
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

//T_AgentiaExecute  执行列表
//T_AgentiaSaving 试剂列表
void Dg_TakeOutPage::Write_Sql(QMap<int, QString> map, QString &name)
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

void Dg_TakeOutPage::ShowExecuteWindow()
{
    Dg_ExecTakeOut *execTakeOut = new Dg_ExecTakeOut(this);
    execTakeOut->show();
    this->hide();
    resetSignal = HALTQUITPROCESS;//停止自动退出功能

    connect(execTakeOut, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

void Dg_TakeOutPage::DownLoad_TaskList()
{
    QSqlQuery query;
    query.exec(QString("DELETE from T_AgentiaExecute"));

    updateSQL->GetAllAgentia(&userId);
//    updateSQL->GetTaskList(&userId, 1);
}

void Dg_TakeOutPage::SetTitle(QString name)
{
    ui->LB_title->setText(QStringLiteral("%1").arg(name));
}


void Dg_TakeOutPage::Del(QTableWidget *source)
{
    QList<QTableWidgetItem *> array = source->selectedItems();
    int count = array.count();
    int num = 0;
    QList<int> rowNum;
    rowNum.append(-1);

    for (int i = 0; i < count; i++)
    {
        int row = source->row(array.at(i));
        if (rowNum[num] != row)
        {
            if (source->item(row, 2)->text() == "在位")
            {
                rowNum.append(row);
                num++;
            }
        }
    }

    if (rowNum.length() > 1)
    {
        source->removeRow(rowNum[1]);//删除第一条
    }
    for (int i = 2; i < rowNum.length(); i++)
    {
        source->removeRow(rowNum[i] - (i - 1));
    }
}

void Dg_TakeOutPage::Add(QTableWidget *goal, QTableWidget *source)
{
    QList<QTableWidgetItem *> array = source->selectedItems();
    int count = array.count();
    int old = -1;
    int current = -1;
    int nowCount = -1;
    int rowCount = -1;

    for (int i = 0; i < count; i++)
    {

        int row = source->row(array.at(i));
        QTableWidgetItem *item = array.at(i);
        QString name = item->text();

        current = row;
        if (source->item(current, 2)->text() == "在位")
        {
            if (current != old)//当前行号不等于之前存储行号说明需要新增一行
            {
                rowCount = goal->rowCount();
                goal->setRowCount(rowCount + 1);
                old = current;//存储当前行号
                nowCount = 0;//重置列数 应对：上一行数据不全导致列数少实际总的需要列数

            }
            else
            {
                nowCount++;//需要新增一列
                int columnCount = goal->columnCount();//当前表格列数
                if (columnCount <= nowCount)//需求大于实际表格具备的列数则新增一列
                {
                    goal->setColumnCount(columnCount + 1);
                }
            }

            if (0 == nowCount)//第一列作为排序，需要插入整型变量
            {
                QTableWidgetItem *test = new QTableWidgetItem();//插入整型
                int integer = name.toInt();
                qDebug()<<integer;
                test->setData(Qt::DisplayRole, integer);
                goal->setItem(rowCount,nowCount,test);
            }
            else
            {
                goal->setItem((rowCount),(nowCount),new QTableWidgetItem(name));
            }
        }
    }
}























