#include "operatewindow.h"
#include "ui_operatewindow.h"


OperateWindow::OperateWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OperateWindow)
{
    ui->setupUi(this);

    //    this->showFullScreen();
//        showMaximized();
//        setWindowFlags(Qt::FramelessWindowHint);

    T_table = &T_TABLE;
    query                = new QSqlQuery;
    T_model_Other_Execut = new QSqlTableModel;
    T_model_Other_Search = new QSqlTableModel;

    delayTime = new DelayTime;
    http_GP = new HttpGP;
    Operate_execut_Page  = new Execut_window;
    AgentiaInfoChange_execute_Page = new AgentiaInfoChange;
    Sheet_Operate_execute_Page = new Sheet_OperatePage;

    pHeader = new MyQHeaderView(Qt::Horizontal,this);
    EHeader = new ExecuteSheet(Qt::Horizontal,this);
    checkBoxDelegate = new CheckBoxDelegate(this);
    returnAgentiaChechBoxDelegate = new ReturnAgentiaCheckBoxDelegate(this);

    add_NewOperate = new AddNew(this);
    del_NewOperate = new DelNew(this);
    change_NewOperate = new ChangeNew(this);
    blake_Operate = new BlakeDelegate(this);

    ui->tableView_showExecuteInfo->setSelectionMode(QAbstractItemView::MultiSelection);//多选
    ui->tableView_showExecuteInfo->setSelectionBehavior(QAbstractItemView::SelectRows);//每次选择一行



    ui->tableView_showSearchInfo->setHorizontalHeader(pHeader);
    ui->tableView_showSearchInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_showSearchInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);//窗口不可编辑

    ui->tableView_showExecuteInfo->setHorizontalHeader(EHeader);
    ui->tableView_showExecuteInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_showExecuteInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);//窗口不可编辑

    connect(pHeader,SIGNAL(stateUP(int)),this,SLOT(onState(int)));//头改子列
    connect(EHeader,SIGNAL(stateUP_ES(int)),this,SLOT(onState(int)));

    connect(checkBoxDelegate,SIGNAL(headUP(int)),pHeader,SLOT(SetHeaderBox(int)));//子改头列
    connect(checkBoxDelegate,SIGNAL(headUP(int)),EHeader,SLOT(SetHeaderBox(int)));

    connect(this,SIGNAL(stateUp_OperatePage(int)),pHeader,SLOT(SetHeaderBox(int)));//子改头列
    connect(this,SIGNAL(stateUp_OperatePage(int)),EHeader,SLOT(SetHeaderBox(int)));

    connect(add_NewOperate,SIGNAL(sendNewFromPBT(int)),this,SLOT(receiveAddNew(int)));//添加信息
    connect(del_NewOperate,SIGNAL(sendNewFromPBT(int)),this,SLOT(receiveAddNew(int)));//删除信息
    connect(change_NewOperate,SIGNAL(sendNewFromPBT(int)),this,SLOT(receiveAddNew(int)));//修改信息

    connect(this,SIGNAL(sendInfo_To_AgentiaInfoChange(int,int,int)),AgentiaInfoChange_execute_Page,SLOT(sendUP_Info(int,int,int)));//更新状态
    connect(this,SIGNAL(sendInfo_To_agentiaInfoChangePageForEnable(int)),AgentiaInfoChange_execute_Page,SLOT(receiverUpEnable(int)));//初始化显示状态

    connect(AgentiaInfoChange_execute_Page,SIGNAL(upSheet_PutIn()),this,SLOT(upSheet_PutInWindow()));//修改界面更新表格
    connect(Operate_execut_Page,SIGNAL(upSheet_From_Execute()),this,SLOT(upSheet_PutInWindow()));//执行界面更新表格
    connect(Operate_execut_Page,SIGNAL(upSheet_From_Execute()),this,SIGNAL(OperateWindow_To_MainUI()));//返回主界面

    connect(Sheet_Operate_execute_Page,SIGNAL(closePrevious()),this,SLOT(closePage()));

    connect(http_GP,SIGNAL(sendError_To_Operate()),this,SLOT(OperateError_info()));//反馈信息



}



void OperateWindow::onState(int state)
{
    QString setInfo;
    int countRow ;
    if((state == 1) || (state == 0))
    {
        countRow = T_model_Other_Search->rowCount();
        if(state == 1)
        {
            setInfo = "选择";
        }
        else
        {
            setInfo = "未选择";
        }

        for(int i = 0; i < countRow; i++)
        {
            T_model_Other_Search->setData(T_model_Other_Search->index(i,1),QString("%1").arg(setInfo));
        }
    }
    else if((state == 2) || (state == 3))
    {
        countRow = T_model_Other_Execut->rowCount();
        if(state == 3)
        {
            setInfo = "选择";
        }
        else
        {
            setInfo = "未选择";
        }

        for(int i = 0; i < countRow; i++)
        {
            T_model_Other_Execut->setData(T_model_Other_Execut->index(i,1),QString("%1").arg(setInfo));
        }

    }
}


OperateWindow::~OperateWindow()
{
    delete ui;
}

/********************************/
void OperateWindow::on_Bt_backBefore_clicked()
{
    closePage();
}


void OperateWindow::on_Bt_searchInfo_clicked()
{
    QString agentiaName = ui->lineEdit_getSearchInfo->text();//获取搜索框的内容

    if(agentiaName == "")
    {
        if(T_table->dialog_model == 1)
        {
            tableInit(1);
        }
        else if(T_table->dialog_model == 2)
        {
            tableInit(2);
        }

    }
    else
    {
        T_model_Other_Search->setFilter(QString("agentiaName like '%%1%'").arg(agentiaName));//筛选结果
    }
}

void OperateWindow::on_Bt_execute_clicked()
{
    updateNo(1);
    if ((0 == T_table->executeCount) && (1 == T_table->dialog_model))
    {
        if(checkSheet(2))//检查是否有未填写试剂信息
        {
            requestPositionInfo();
        }
    }
    else
    {
        T_table->executeCount = 0;

        if(checkSheet(2))//检查是否有未填写试剂信息
        {
            if(T_table->dialog_model == 1)
            {

                  checkSheet(1);
                  if(T_table->notDrawer != 0)
                  {
                      QMessageBox::StandardButton reply;
                      reply = QMessageBox::question(this, "Question", QString("是否跳过没填位置试剂,有%1个位置").arg(T_table->notDrawer),
                                                    QMessageBox::Yes|QMessageBox::No);

                      if (reply == QMessageBox::Yes)
                      {

                          int allrow = T_model_Other_Execut->rowCount();
                          QString getC_status = "";

                          for(int i = 0 ; i < allrow; i++)//去除不符合标准的试剂信息
                          {
                                getC_status = T_model_Other_Execut->data(T_model_Other_Execut->index(i,4)).toString();
                                if(getC_status != "已分配位置")
                                {
                                    T_model_Other_Execut->removeRow(i);

                                    qDebug()<<"删除 i";
                                }


                          }
                          T_model_Other_Execut->submitAll();

                          allrow = T_model_Other_Execut->rowCount();//重新排序

                          for(int i = 0 ; i < allrow; i++)
                          {
                              T_model_Other_Execut->setData(T_model_Other_Execut->index(i,0),i+1);
                          }

                          T_model_Other_Execut->submitAll();

                          if(allrow == 0)
                          {
                              QMessageBox qMebox;
                              qMebox.setText("没有可执行信息");
                              qMebox.exec();
                          }
                          else
                          {
                              Operate_execut_Page->executeInit(1);
                              Operate_execut_Page->exec();
                          }
                      }
                  }
                  else
                  {
                      Operate_execut_Page->executeInit(1);
                      Operate_execut_Page->exec();
                  }

            }
            else if ((T_table->dialog_model == 2) || (T_table->dialog_model == 5))//报废//取
            {

               Sheet_Operate_execute_Page->modelOption(T_table->dialog_model);
               Sheet_Operate_execute_Page->exec();

            }
            else if((T_table->dialog_model == 4) || (T_table->dialog_model == 6))//点验//替换
            {
                Operate_execut_Page->executeInit(T_table->dialog_model);
                Operate_execut_Page->exec();
            }
        }
    }
}

void OperateWindow::on_Bt_delete_clicked()
{
    int countRow = T_model_Other_Execut->rowCount();

    for(int i = 0;i < countRow; i++)
    {
        QString getValue = T_model_Other_Execut->data(T_model_Other_Execut->index(i,1)).toString();
        if(getValue == "选择")
        {
            delInfo_Execute(i);
        }
    }

    emit stateUp_OperatePage(1);

    T_model_Other_Search->submitAll();
    T_model_Other_Execut->submitAll();

}

void OperateWindow::on_Bt_add_clicked()
{

    int countRow_S = T_model_Other_Search->rowCount();
    int rowNum = 0;
    int countRow_P = T_model_Other_Execut->rowCount();

    rowNum = countRow_P;

    for(int i = 0;i < countRow_S; i++)
    {
        QString getValue = T_model_Other_Search->data(T_model_Other_Search->index(i,1)).toString();
        if(getValue == "选择")
        {
            addInfo_Search_To_Execute(i,rowNum);
            rowNum++;

        }

    }
    emit stateUp_OperatePage(1);

    T_model_Other_Search->submitAll();
    T_model_Other_Execut->submitAll();

}

void OperateWindow::on_Bt_changeInfo_A_clicked()//修改按钮
{


    if(T_table->dialog_model == 4)//替换
    {
        emit sendInfo_To_agentiaInfoChangePageForEnable(0);
    }
    else if(T_table->dialog_model == 6)//报废
    {
        emit sendInfo_To_agentiaInfoChangePageForEnable(2);
    }
    else if(T_table->dialog_model == 1)//入柜
    {
        emit sendInfo_To_agentiaInfoChangePageForEnable(1);
    }

     emit sendInfo_To_AgentiaInfoChange(0,2,0);//集体模式 status = 0, OK_Push = 2
       AgentiaInfoChange_execute_Page->exec();
}


/**************************************************/
void OperateWindow::ModelSelect(int num)//1:入柜 2：取 3：还 4:替换 5:报废 6：点验
{
    switch (num) {
    case 1 :{
        T_table->T_execut_RelationTable = "T_Task_PutIn";
        T_table->T_search_RelationTable = "T_AgentiaTypeList";
        T_table->dialog_model = 1;
        ui->label_title->setText("入柜");
        ui->Bt_changeInfo_A->show();
        tableInit(1);
        break;
    }
    case 2:{
        T_table->T_execut_RelationTable = "T_AgentiaExecute";
        T_table->T_search_RelationTable = "T_AgentiaSaving";
        T_table->dialog_model = 2;
        ui->label_title->setText("取试剂");
        ui->Bt_changeInfo_A->hide();
        tableInit(2);
        break;
    }
    case 4:{
        T_table->T_execut_RelationTable = "T_AgentiaReplace";
        T_table->T_search_RelationTable = "T_AgentiaSaving" ;
        T_table->dialog_model = 4;
        ui->label_title->setText("替换试剂");
        ui->Bt_changeInfo_A->show();
        tableInit(4);
        break;
    }
    case 5:{
        T_table->T_execut_RelationTable = "T_AgentiaScrap";
        T_table->T_search_RelationTable = "T_AgentiaSaving";
        T_table->dialog_model = 5;
        ui->label_title->setText("报废试剂");
        ui->Bt_changeInfo_A->hide();
        tableInit(5);
        break;
    }
    case 6:{
        T_table->T_execut_RelationTable = "T_AgentiaCheck";
        T_table->T_search_RelationTable = "T_AgentiaSaving";
        T_table->dialog_model = 6;
        ui->label_title->setText("点验试剂");
        ui->Bt_changeInfo_A->show();
        tableInit(6);
        break;
    }
    default:
        break;
    }

    T_table->executeCount = 0;
    updateNo(2);//更新表格序列号为之后的增减做准备
    /*************************************/

}


void OperateWindow::receiveAddNew(int option)//1：添加；2：删除；3：修改
{
    int currentRow;
    if(option == 1)//添加
    {
        int countrow = T_model_Other_Execut->rowCount();
        currentRow = ui->tableView_showSearchInfo->currentIndex().row();
        addInfo_Search_To_Execute(currentRow,countrow);
    }
    else if(option == 2)
    {
        currentRow = ui->tableView_showExecuteInfo->currentIndex().row();
        delInfo_Execute(currentRow);
    }
    else if(option == 3)
    {
        currentRow = ui->tableView_showExecuteInfo->currentIndex().row();

        if(T_table->dialog_model == 4)//替换 注销部分输入框编辑功能
        {
            emit sendInfo_To_agentiaInfoChangePageForEnable(0);
        }
        else if(T_table->dialog_model == 6)//报废
        {
            emit sendInfo_To_agentiaInfoChangePageForEnable(2);
        }
        else if(T_table->dialog_model == 1)//入柜 使能部分输入框功能
        {
            emit sendInfo_To_agentiaInfoChangePageForEnable(1);
        }

        emit sendInfo_To_AgentiaInfoChange(1,1,currentRow);//单个模式
        AgentiaInfoChange_execute_Page->exec();
    }

    if(T_model_Other_Execut->submitAll());
    else
    {
        qDebug()<<T_model_Other_Execut->lastError().text()<<"E";
    }

    if(T_model_Other_Search->submitAll());
    else
    {
        qDebug()<<T_model_Other_Search->lastError().text()<<"S";
    }


}

void OperateWindow::addInfo_Search_To_Execute(int i, int rowNum)//将对应的信息添加到执行框
{


    if(T_table->dialog_model == 1)//入柜操作中
    {

        T_model_Other_Search->setData(T_model_Other_Search->index(i,1),QString("未选择"));

        QString getC_AName = T_model_Other_Search->data(T_model_Other_Search->index(i,2)).toString();
        uint getC_Attribute = T_model_Other_Search->data(T_model_Other_Search->index(i,7)).toInt();
        uint getC_ID = T_model_Other_Search->data(T_model_Other_Search->index(i,8)).toInt();
        QString getC_LFormula = T_model_Other_Search->data(T_model_Other_Search->index(i,4)).toString();
        QString getC_Specification = T_model_Other_Search->data(T_model_Other_Search->index(i,5)).toString();
        QString getC_Factory = T_model_Other_Search->data(T_model_Other_Search->index(i,6)).toString();
        QString getC_ItemNo = T_model_Other_Search->data(T_model_Other_Search->index(i,9)).toString();

        T_model_Other_Execut->insertRow(rowNum);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,0),rowNum+1);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,1),QString("未选择"));
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,2),QString("修改"));
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,3),getC_AName);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,4),QString("未操作"));
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,9),QString("%1").arg(0));
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,10),getC_Attribute);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,11),getC_ID);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,12),getC_LFormula);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,13),getC_Specification);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,14),getC_Factory);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,15),getC_ItemNo);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,16),0);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,17),0);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,18),0);

    }
    else if(T_table->dialog_model == 2 || T_table->dialog_model == 5)//取
    {

        T_model_Other_Search->setData(T_model_Other_Search->index(i,1),QString("未选择"));

        int getC_No = T_model_Other_Search->data(T_model_Other_Search->index(i,0)).toInt();
        QString getC_AName = T_model_Other_Search->data(T_model_Other_Search->index(i,2)).toString();
        QString getC_bottleCapacity = T_model_Other_Search->data(T_model_Other_Search->index(i,3)).toString();
        QString getC_dose = T_model_Other_Search->data(T_model_Other_Search->index(i,4)).toString();
        int getC_drawerNo = T_model_Other_Search->data(T_model_Other_Search->index(i,5)).toInt();
        int getC_positionNo = T_model_Other_Search->data(T_model_Other_Search->index(i,6)).toInt();
        QString getC_expireDate = T_model_Other_Search->data(T_model_Other_Search->index(i,7)).toString();
        int getC_attribute = T_model_Other_Search->data(T_model_Other_Search->index(i,8)).toInt();
        int getC_AID = T_model_Other_Search->data(T_model_Other_Search->index(i,9)).toInt();
        int getC_positionID = T_model_Other_Search->data(T_model_Other_Search->index(i,10)).toInt();

        T_model_Other_Execut->insertRow(rowNum);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,0),getC_No);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,1),QString("未选择"));
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,2),getC_AName);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,3),getC_bottleCapacity);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,4),getC_dose);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,5),getC_drawerNo);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,6),getC_positionNo);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,7),getC_expireDate);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,8),getC_attribute);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,9),getC_AID);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,10),getC_positionID);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,11),QString("未摆放"));

        T_model_Other_Search->removeRow(i);
    }
    else if((T_table->dialog_model == 4) || (T_table->dialog_model == 6))//替换  点验
    {

        T_model_Other_Search->setData(T_model_Other_Search->index(i,1),QString("未选择"));

        int getC_No = T_model_Other_Search->data(T_model_Other_Search->index(i,0)).toInt();
        QString getC_AName = T_model_Other_Search->data(T_model_Other_Search->index(i,2)).toString();
        QString getC_bottleCapacity = T_model_Other_Search->data(T_model_Other_Search->index(i,3)).toString();
        QString getC_dose = T_model_Other_Search->data(T_model_Other_Search->index(i,4)).toString();
        int getC_drawerNo = T_model_Other_Search->data(T_model_Other_Search->index(i,5)).toInt();
        int getC_positionNo = T_model_Other_Search->data(T_model_Other_Search->index(i,6)).toInt();
        QString getC_expireDate = T_model_Other_Search->data(T_model_Other_Search->index(i,7)).toString();
        int getC_attribute = T_model_Other_Search->data(T_model_Other_Search->index(i,8)).toInt();
        int getC_AID = T_model_Other_Search->data(T_model_Other_Search->index(i,9)).toInt();
        int getC_positionID = T_model_Other_Search->data(T_model_Other_Search->index(i,10)).toInt();

        T_model_Other_Execut->insertRow(rowNum);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,0),getC_No);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,1),QString("未选择"));
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,2),QString("修改"));
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,3),getC_AName);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,4),getC_bottleCapacity);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,5),getC_dose);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,7),getC_expireDate);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,8),getC_drawerNo);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,9),getC_positionNo);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,10),getC_attribute);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,11),getC_AID);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,12),getC_positionID);
        T_model_Other_Execut->setData(T_model_Other_Execut->index(rowNum,13),QString("未摆放"));

        T_model_Other_Search->removeRow(i);
    }
}

void OperateWindow::delInfo_Execute(int i)
{
    if(T_table->dialog_model == 1)//删除信息
    {
        int getC_attribute = T_model_Other_Execut->data(T_model_Other_Execut->index(i,9)).toInt();
        if (ALLOWOPERATE == getC_attribute)
        {
            return ;
        }

        T_model_Other_Execut->removeRow(i);
    }
    else if(T_table->dialog_model == 2 || T_table->dialog_model == 5)//取  删除添加到另一处
    {
        T_model_Other_Execut->setData(T_model_Other_Execut->index(i,1),QString("未选择"));

        int getC_No = T_model_Other_Execut->data(T_model_Other_Execut->index(i,0)).toInt();
        QString getC_AName = T_model_Other_Execut->data(T_model_Other_Execut->index(i,2)).toString();
        QString getC_bottleCapacity = T_model_Other_Execut->data(T_model_Other_Execut->index(i,3)).toString();
        QString getC_dose = T_model_Other_Execut->data(T_model_Other_Execut->index(i,4)).toString();
        int getC_drawerNo = T_model_Other_Execut->data(T_model_Other_Execut->index(i,5)).toInt();
        int getC_positionNo = T_model_Other_Execut->data(T_model_Other_Execut->index(i,6)).toInt();
        QString getC_expireDate = T_model_Other_Execut->data(T_model_Other_Execut->index(i,7)).toString();
        int getC_attribute = T_model_Other_Execut->data(T_model_Other_Execut->index(i,8)).toInt();
        int getC_AID = T_model_Other_Execut->data(T_model_Other_Execut->index(i,9)).toInt();
        int getC_positionID = T_model_Other_Execut->data(T_model_Other_Execut->index(i,10)).toInt();
        QString getC_judgeA = T_model_Other_Execut->data(T_model_Other_Execut->index(i,11)).toString();

        if (ALLOWOPERATE == getC_attribute)
        {
            return ;
        }

        int rowNum = T_model_Other_Search->rowCount();

        T_model_Other_Search->insertRow(rowNum);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,0),getC_No);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,1),QString("未选择"));
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,2),getC_AName);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,3),getC_bottleCapacity);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,4),getC_dose);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,5),getC_drawerNo);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,6),getC_positionNo);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,7),getC_expireDate);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,8),getC_attribute);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,9),getC_AID);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,10),getC_positionID);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,11),getC_judgeA);

        T_model_Other_Execut->removeRow(i);

    }
    else if((T_table->dialog_model == 4) || (T_table->dialog_model == 6))//替换 点验
    {
        T_model_Other_Execut->setData(T_model_Other_Execut->index(i,1),QString("未选择"));

        int getC_No = T_model_Other_Execut->data(T_model_Other_Execut->index(i,0)).toInt();
        QString getC_AName = T_model_Other_Execut->data(T_model_Other_Execut->index(i,3)).toString();
        QString getC_bottleCapacity = T_model_Other_Execut->data(T_model_Other_Execut->index(i,4)).toString();
        QString getC_dose = T_model_Other_Execut->data(T_model_Other_Execut->index(i,5)).toString();
        QString getC_expireDate = T_model_Other_Execut->data(T_model_Other_Execut->index(i,7)).toString();
        int getC_drawerNo = T_model_Other_Execut->data(T_model_Other_Execut->index(i,8)).toInt();
        int getC_positionNo = T_model_Other_Execut->data(T_model_Other_Execut->index(i,9)).toInt();
        int getC_attribute = T_model_Other_Execut->data(T_model_Other_Execut->index(i,10)).toInt();
        int getC_AID = T_model_Other_Execut->data(T_model_Other_Execut->index(i,11)).toInt();
        int getC_positionID = T_model_Other_Execut->data(T_model_Other_Execut->index(i,12)).toInt();

        int rowNum = T_model_Other_Search->rowCount();

        if (ALLOWOPERATE == getC_attribute)
        {
            return ;
        }

        T_model_Other_Search->insertRow(rowNum);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,0),getC_No);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,1),QString("未选择"));
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,2),getC_AName);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,3),getC_bottleCapacity);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,4),getC_dose);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,5),getC_drawerNo);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,6),getC_positionNo);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,7),getC_expireDate);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,8),getC_attribute);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,9),getC_AID);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,10),getC_positionID);
        T_model_Other_Search->setData(T_model_Other_Search->index(rowNum,11),QString("未操作"));

        T_model_Other_Execut->removeRow(i);

    }

}

void OperateWindow::upSheet_PutInWindow()
{
    T_model_Other_Execut->select();
}

void OperateWindow::closePage()
{
    emit OperateWindow_To_MainUI();
    delayTime->waitTaskInfo(100);
    this->close();
}



void OperateWindow::updateNo(int order)//更新数据表格行列号
//1：执行框表格序列号 2：顺序重写两个表格顺序号
{
    int E_allRow = 0;//执行表格的行数
    int S_allRow = 0;//待选表格的行数

    E_allRow = T_model_Other_Execut->rowCount();
    S_allRow = T_model_Other_Search->rowCount();

    if(order == 1)
    {
        for(int i = 0 ; i < E_allRow ; i++)
        {
            T_model_Other_Execut->setData(T_model_Other_Execut->index(i,0),i+1);

        }

        T_model_Other_Execut->submitAll();
    }
    else if(order == 2)
    {
        updateNo(1);

        for(int i = 0 ; i < S_allRow; i++)
        {
            T_model_Other_Search->setData(T_model_Other_Search->index(i,0),E_allRow+i+1);
        }

        T_model_Other_Search->submitAll();
    }
}

void OperateWindow::requestPositionInfo(void)
{
    int allRowCount = T_model_Other_Execut->rowCount();
    http_GP->JsonForSend(5, T_table->T_execut_RelationTable, allRowCount);
}

void OperateWindow::OperateError_info(void)//显示更新信息
{
    T_model_Other_Execut->select();
    T_table->executeCount = 1;
    on_Bt_execute_clicked();
}


int OperateWindow::checkSheet(int order)//检查数据表格关键位置信息是否完善
// 0：检查size  1：检查position And drawer
{
    QString getC_status = "" ;
    QString getC_size = "";
    QString getC_bottle = "";
    QString getC_dose = "";
    QString getC_expireDate = "";
    QString getC_newDose = "";
    int     getC_position = 0;
    int     getC_drawer   = 0;
    int rowAll = T_model_Other_Execut->rowCount();

    if(order == 1)//1：执行
    {
        T_table->notDrawer = 0;

        for(int i = 0 ; i < rowAll ; i++)
        {
            getC_position = T_model_Other_Execut->data(T_model_Other_Execut->index(i,18)).toInt();
            getC_drawer = T_model_Other_Execut->data(T_model_Other_Execut->index(i,17)).toInt();

            if(getC_position == 0 && getC_drawer == 0)
            {
                T_model_Other_Execut->setData(T_model_Other_Execut->index(i,4),"未分配位置");
                T_table->notDrawer++;
            }
        }

        T_model_Other_Execut->submitAll();
    }

    else if(order == 2)//2:检查试剂其他信息是否完善
    {
        if(T_table->dialog_model == 1)
        {
            for(int i = 0 ; i < rowAll ; i++)
            {

                getC_bottle = T_model_Other_Execut->data(T_model_Other_Execut->index(i,5)).toString();
                getC_dose = T_model_Other_Execut->data(T_model_Other_Execut->index(i,6)).toString();
                getC_size = T_model_Other_Execut->data(T_model_Other_Execut->index(i,7)).toString();
                getC_expireDate = T_model_Other_Execut->data(T_model_Other_Execut->index(i,8)).toString();
                getC_status = T_model_Other_Execut->data(T_model_Other_Execut->index(i,4)).toString();

                if ((getC_status == "已分配位置") || (getC_status == "已审批"))
                {
                    ;
                }
                else if ((getC_size == "")||(getC_bottle == "")||(getC_dose == "")||(getC_expireDate == ""))
                {
                    QMessageBox qMbox;
                    qMbox.setText(QString("第 %1 行未填写信息").arg(i+1));
                    qMbox.exec();
                    return 0;
                }
            }
        }
        else if(T_table->dialog_model == 4)
        {
            for(int i = 0; i < rowAll; i++)
            {
                getC_newDose = T_model_Other_Execut->data(T_model_Other_Execut->index(i,6)).toString();
                if(getC_newDose == "")
                {
                    QMessageBox qMbox;
                    qMbox.setText(QString("第 %1 行未填写信息").arg(i+1));
                    qMbox.exec();
                    return 0;
                }
            }
        }
    }

    return 1;
}



void OperateWindow::tableInit(char modelOption)//1:入柜 2：取 4：替换 5:报废
{
    if(modelOption == 1)//入柜
    {
        T_model_Other_Search->setTable(QString("%1").arg(T_table->T_search_RelationTable));
        T_model_Other_Search->select();
        T_model_Other_Search->setHeaderData(0,Qt::Horizontal,QObject::tr("添加"));
        T_model_Other_Search->setHeaderData(1,Qt::Horizontal,QObject::tr("勾选"));
        T_model_Other_Search->setHeaderData(2,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_Other_Search->setHeaderData(3,Qt::Horizontal,QObject::tr("俗名"));
        T_model_Other_Search->setHeaderData(4,Qt::Horizontal,QObject::tr("化学式"));
        T_model_Other_Search->setHeaderData(5,Qt::Horizontal,QObject::tr("详细介绍"));
        T_model_Other_Search->setHeaderData(6,Qt::Horizontal,QObject::tr("厂商"));
        T_model_Other_Search->setHeaderData(7,Qt::Horizontal,QObject::tr("状态"));
        T_model_Other_Search->setHeaderData(8,Qt::Horizontal,QObject::tr("试剂ID"));
        T_model_Other_Search->setHeaderData(9,Qt::Horizontal,QObject::tr("项目编号"));

        T_model_Other_Search->setEditStrategy(QSqlTableModel::OnManualSubmit);//自动提交

        ui->tableView_showSearchInfo->setModel(T_model_Other_Search);//关联窗口
        ui->tableView_showSearchInfo->setItemDelegateForColumn(0,add_NewOperate);
        ui->tableView_showSearchInfo->setItemDelegateForColumn(1,checkBoxDelegate);

        /**********************************************************************/
        T_model_Other_Execut->setTable(QString("%1").arg(T_table->T_execut_RelationTable));
        T_model_Other_Execut->select();

        T_model_Other_Execut->setHeaderData(0,Qt::Horizontal,QObject::tr("删除"));
        T_model_Other_Execut->setHeaderData(1,Qt::Horizontal,QObject::tr("勾选"));
        T_model_Other_Execut->setHeaderData(2,Qt::Horizontal,QObject::tr("修改"));
        T_model_Other_Execut->setHeaderData(3,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_Other_Execut->setHeaderData(4,Qt::Horizontal,QObject::tr("状态"));
        T_model_Other_Execut->setHeaderData(5,Qt::Horizontal,QObject::tr("额定容量"));
        T_model_Other_Execut->setHeaderData(6,Qt::Horizontal,QObject::tr("实际容量"));
        T_model_Other_Execut->setHeaderData(7,Qt::Horizontal,QObject::tr("规格"));
        T_model_Other_Execut->setHeaderData(8,Qt::Horizontal,QObject::tr("失效日期"));
        T_model_Other_Execut->setHeaderData(9,Qt::Horizontal,QObject::tr("任务状态"));
        T_model_Other_Execut->setHeaderData(10,Qt::Horizontal,QObject::tr("试剂类别"));
        T_model_Other_Execut->setHeaderData(11,Qt::Horizontal,QObject::tr("试剂ID"));
        T_model_Other_Execut->setHeaderData(12,Qt::Horizontal,QObject::tr("化学式"));
        T_model_Other_Execut->setHeaderData(13,Qt::Horizontal,QObject::tr("详细介绍"));
        T_model_Other_Execut->setHeaderData(14,Qt::Horizontal,QObject::tr("厂商"));
        T_model_Other_Execut->setHeaderData(15,Qt::Horizontal,QObject::tr("项目编号"));
        T_model_Other_Execut->setHeaderData(16,Qt::Horizontal,QObject::tr("位置ID"));
        T_model_Other_Execut->setHeaderData(17,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_Other_Execut->setHeaderData(18,Qt::Horizontal,QObject::tr("位置"));

        T_model_Other_Execut->setEditStrategy(QSqlTableModel::OnManualSubmit);//自动提交

        ui->tableView_showExecuteInfo->setModel(T_model_Other_Execut);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(0,del_NewOperate);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(2,change_NewOperate);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(1,checkBoxDelegate);
    }
    else if(modelOption == 2 || modelOption == 5)//取试剂 报废试剂
    {
        T_model_Other_Search->setTable(QString("%1").arg(T_table->T_search_RelationTable));
        T_model_Other_Search->select();
        T_model_Other_Search->setHeaderData(0,Qt::Horizontal,QObject::tr("添加"));
        T_model_Other_Search->setHeaderData(1,Qt::Horizontal,QObject::tr("勾选"));
        T_model_Other_Search->setHeaderData(2,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_Other_Search->setHeaderData(3,Qt::Horizontal,QObject::tr("额定容积"));
        T_model_Other_Search->setHeaderData(4,Qt::Horizontal,QObject::tr("试剂容量"));
        T_model_Other_Search->setHeaderData(5,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_Other_Search->setHeaderData(6,Qt::Horizontal,QObject::tr("位置"));
        T_model_Other_Search->setHeaderData(7,Qt::Horizontal,QObject::tr("到期日期"));
        T_model_Other_Search->setHeaderData(8,Qt::Horizontal,QObject::tr("试剂类别"));
        T_model_Other_Search->setHeaderData(9,Qt::Horizontal,QObject::tr("试剂ID"));
        T_model_Other_Search->setHeaderData(10,Qt::Horizontal,QObject::tr("位置ID"));
        T_model_Other_Search->setHeaderData(11,Qt::Horizontal,QObject::tr("状态"));

        T_model_Other_Search->setEditStrategy(QSqlTableModel::OnManualSubmit);//自动提交

        ui->tableView_showSearchInfo->setModel(T_model_Other_Search);//关联窗口
        ui->tableView_showSearchInfo->setItemDelegateForColumn(0,add_NewOperate);
        ui->tableView_showSearchInfo->setItemDelegateForColumn(1,checkBoxDelegate);

        /**********************************************************************/
        T_model_Other_Execut->setTable(QString("%1").arg(T_table->T_execut_RelationTable));
        T_model_Other_Execut->select();

        T_model_Other_Execut->setHeaderData(0,Qt::Horizontal,QObject::tr("添加"));
        T_model_Other_Execut->setHeaderData(1,Qt::Horizontal,QObject::tr("勾选"));
        T_model_Other_Execut->setHeaderData(2,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_Other_Execut->setHeaderData(3,Qt::Horizontal,QObject::tr("额定容积"));
        T_model_Other_Execut->setHeaderData(4,Qt::Horizontal,QObject::tr("试剂容量"));
        T_model_Other_Execut->setHeaderData(5,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_Other_Execut->setHeaderData(6,Qt::Horizontal,QObject::tr("位置"));
        T_model_Other_Execut->setHeaderData(7,Qt::Horizontal,QObject::tr("到期日期"));
        T_model_Other_Execut->setHeaderData(8,Qt::Horizontal,QObject::tr("试剂类别"));
        T_model_Other_Execut->setHeaderData(9,Qt::Horizontal,QObject::tr("试剂ID"));
        T_model_Other_Execut->setHeaderData(10,Qt::Horizontal,QObject::tr("位置ID"));
        T_model_Other_Execut->setHeaderData(11,Qt::Horizontal,QObject::tr("状态"));

        T_model_Other_Execut->setEditStrategy(QSqlTableModel::OnManualSubmit);//自动提交

        ui->tableView_showExecuteInfo->setModel(T_model_Other_Execut);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(0,del_NewOperate);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(2,blake_Operate);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(1,checkBoxDelegate);

    }
    else if((modelOption == 4) || (modelOption == 6))//替换  点验
    {
        T_model_Other_Search->setTable(QString("%1").arg(T_table->T_search_RelationTable));
        T_model_Other_Search->select();
        T_model_Other_Search->setHeaderData(0,Qt::Horizontal,QObject::tr("添加"));
        T_model_Other_Search->setHeaderData(1,Qt::Horizontal,QObject::tr("勾选"));
        T_model_Other_Search->setHeaderData(2,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_Other_Search->setHeaderData(3,Qt::Horizontal,QObject::tr("额定容积"));
        T_model_Other_Search->setHeaderData(4,Qt::Horizontal,QObject::tr("试剂容量"));
        T_model_Other_Search->setHeaderData(5,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_Other_Search->setHeaderData(6,Qt::Horizontal,QObject::tr("位置"));
        T_model_Other_Search->setHeaderData(7,Qt::Horizontal,QObject::tr("到期日期"));
        T_model_Other_Search->setHeaderData(8,Qt::Horizontal,QObject::tr("试剂类别"));
        T_model_Other_Search->setHeaderData(9,Qt::Horizontal,QObject::tr("试剂ID"));
        T_model_Other_Search->setHeaderData(10,Qt::Horizontal,QObject::tr("位置ID"));
        T_model_Other_Search->setHeaderData(11,Qt::Horizontal,QObject::tr("状态"));

        T_model_Other_Search->setEditStrategy(QSqlTableModel::OnManualSubmit);//自动提交

        ui->tableView_showSearchInfo->setModel(T_model_Other_Search);//关联窗口
        ui->tableView_showSearchInfo->setItemDelegateForColumn(0,add_NewOperate);
        ui->tableView_showSearchInfo->setItemDelegateForColumn(1,checkBoxDelegate);

        /**********************************************************************/
        T_model_Other_Execut->setTable(QString("%1").arg(T_table->T_execut_RelationTable));
        T_model_Other_Execut->select();

        T_model_Other_Execut->setHeaderData(0,Qt::Horizontal,QObject::tr("添加"));
        T_model_Other_Execut->setHeaderData(1,Qt::Horizontal,QObject::tr("勾选"));
        T_model_Other_Execut->setHeaderData(2,Qt::Horizontal,QObject::tr("修改"));
        T_model_Other_Execut->setHeaderData(3,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_Other_Execut->setHeaderData(4,Qt::Horizontal,QObject::tr("额定容积"));
        T_model_Other_Execut->setHeaderData(5,Qt::Horizontal,QObject::tr("试剂容量"));
        T_model_Other_Execut->setHeaderData(6,Qt::Horizontal,QObject::tr("新放容量"));
        T_model_Other_Execut->setHeaderData(7,Qt::Horizontal,QObject::tr("到期日期"));
        T_model_Other_Execut->setHeaderData(8,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_Other_Execut->setHeaderData(9,Qt::Horizontal,QObject::tr("位置"));
        T_model_Other_Execut->setHeaderData(10,Qt::Horizontal,QObject::tr("试剂类别"));
        T_model_Other_Execut->setHeaderData(11,Qt::Horizontal,QObject::tr("试剂ID"));
        T_model_Other_Execut->setHeaderData(12,Qt::Horizontal,QObject::tr("位置ID"));
        T_model_Other_Execut->setHeaderData(13,Qt::Horizontal,QObject::tr("状态"));

        T_model_Other_Execut->setEditStrategy(QSqlTableModel::OnManualSubmit);//自动提交

        ui->tableView_showExecuteInfo->setModel(T_model_Other_Execut);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(0,del_NewOperate);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(2,change_NewOperate);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(1,checkBoxDelegate);

    }
}
