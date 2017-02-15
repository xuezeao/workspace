#include "sheet_operatepage.h"
#include "ui_sheet_operatepage.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDesktopWidget>


extern int Alarm_No[3][32];

Sheet_OperatePage::Sheet_OperatePage(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::Sheet_OperatePage)
{
    ui->setupUi(this);
 /*   setWindowTitle(QString("操作界面"));//标题
    setWindowFlags(windowFlags()&~(Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint));
    //最大化等按钮无效化*/
    //    this->showFullScreen();
//        showMaximized();
        setWindowFlags(Qt::FramelessWindowHint);

    T_tableexecute = new T_table;

//    move((QApplication::desktop()->width()-this->width())/2,(QApplication::desktop()->height()-this->height())/2);//居中

    T_model_execute = new QSqlTableModel;
    http_GAndP_sheetOperate = new HttpGP;

    connect(http_GAndP_sheetOperate,SIGNAL(sendInfo_To_sheetPage()),this,SLOT(receivers_From_http()));//接收来自post讯号
}

Sheet_OperatePage::~Sheet_OperatePage()
{
    delete ui;
}

/********************键盘区**************************/

void Sheet_OperatePage::on_pBt_return_clicked()
{
    sendHttp();
    closeOperate();
}



void Sheet_OperatePage::on_pBt_OK_clicked()
{
    if((sheet_status == 1)||(sheet_status ==3))
    {

        if(sheet_status == 3)
        {
            closeOperate();
        }else{
             sheet_status = 0;
             pBtStatus(2);//查询指令
        }

    }

}

/*******************************************************/
void Sheet_OperatePage::modelOption(int order)
{
    switch (order) {            //取试剂
    case 2:{
        T_tableexecute->T_tablesheet = "T_AgentiaExecute";
        T_tableexecute->modelOperate = 2;
        ui->label_title->setText("取试剂");
        break;
    }
    case 5:{                    //报废试剂
        T_tableexecute->T_tablesheet = "T_AgentiaScrap";
        T_tableexecute->modelOperate = 5;
        ui->label_title->setText("报废试剂");

        break;
    }
    default:
        break;
    }
    ui->pBt_ignore->setEnabled(true);
    ui->pBt_OK->setEnabled(true);
    ui->pBt_return->setEnabled(true);
    sheet_drawer[7] = {0};//存储抽屉号和总数
    sheet_position[17] = {0};//存储当前试剂位置和总数
    InfoError(2,"请继续操作");
    sheetTableInit(1);
    /*************************************/

}

void Sheet_OperatePage::sheetTableInit(int num)
{


    if((num == 1)||(num == 0))//方便再次调用
    {
        T_model_execute->setTable(QString("%1").arg(T_tableexecute->T_tablesheet));
        T_model_execute->select();

        T_model_execute->setHeaderData(2,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_execute->setHeaderData(4,Qt::Horizontal,QObject::tr("试剂容量"));
        T_model_execute->setHeaderData(5,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_execute->setHeaderData(6,Qt::Horizontal,QObject::tr("位置"));
        T_model_execute->setHeaderData(11,Qt::Horizontal,QObject::tr("状态"));

        T_model_execute->setEditStrategy(QSqlTableModel::OnManualSubmit);//自动提交

        T_model_execute->sort(5, Qt::AscendingOrder);//升序，根据抽屉号

        ui->tableView_Execute->setModel(T_model_execute);//关联窗口

        ui->tableView_Execute->setColumnHidden(0,true);//隐藏
        ui->tableView_Execute->setColumnHidden(1,true);
        ui->tableView_Execute->setColumnHidden(3,true);
        ui->tableView_Execute->setColumnHidden(7,true);
        ui->tableView_Execute->setColumnHidden(8,true);
        ui->tableView_Execute->setColumnHidden(9,true);
        ui->tableView_Execute->setColumnHidden(10,true);

        ui->tableView_Execute->setEditTriggers(QAbstractItemView::NoEditTriggers);//窗口不可编辑
        //ui->tableView_Execute->setColumnWidth(2,10);设定columnwidth


    }

    if(num == 1)
    {
        /*********初始化按钮********/

        sheet_model = 0;
        T_tableexecute->rowid = 0;
        sheet_status = 1;

        getDrawer();
        pBtStatus(1);
        T_tableexecute->error = 1;
        /*********************/
        if (T_model_execute->rowCount() == 0)
        {
            closeOperate();
        }
    }



}


void Sheet_OperatePage::changeInfo(int just,int i)//更改试剂信息 1: 上传成功 2：上传失败 3：未操作
{
    QSqlQuery query;
    QString error;
    if(just == 1)
    {
        error = "上传成功";

    }else if(just == 2)
    {
        error = "上传失败";
    }else if(just == 3)
    {
        error = "未操作";
    }


        query.exec(QString("update %3 set judgeAttitude='%1' where rowid=%2").arg(error).arg(i+1).arg(T_tableexecute->T_tablesheet));

}

void Sheet_OperatePage::savePostFalse(QString name, int i)//保存上传失败的指令
{
    if(T_tableexecute->modelOperate == 2 || T_tableexecute->modelOperate == 5)
    {
        QSqlQuery query;
        query.exec(QString("select * from %1").arg(name));
        query.seek(i);
        int getC_agentiaId = query.value(9).toInt();//试剂ID
        int getC_positionId = query.value(10).toInt();//位置ID

        query.exec(QString("select * from T_UserInfo"));
        query.seek(0);
        int getC_userId = query.value(0).toInt();//获取用户ID

        query.exec(QString("select * from T_WaitPostInfo"));
        query.last();
        int getC_all = query.at();//获取数据表格row

        query.prepare("insert into T_WaitPostInfo (id,userId,agentiaId,\
                      positionId,dose,judgeAttitude) values (?,?,?,?,?,?)");
        query.addBindValue(getC_all+1);
        query.addBindValue(getC_userId);
        query.addBindValue(getC_agentiaId);
        query.addBindValue(getC_positionId);
        query.addBindValue("0");
        query.addBindValue("取");
        query.exec();
    }



}


void Sheet_OperatePage::getDrawer()
{

    if(getRead(1))//get drawer count and drawerNo
    {
        QMessageBox qMbox;
        qMbox.setText(QString("数据解析错误"));
        qMbox.exec();
    }

    showNeedData(1,0);


}

void Sheet_OperatePage::updateLabel(int i)//show current drawer
{
    ui->label_allCount->setText(QString("共%1 个任务").arg(sheet_drawer[0]));
    ui->label_current->setText(QString("第%1 个任务").arg(sheet_drawer[i]));

}

int Sheet_OperatePage::getRead(char order)//读写 1:得 抽屉号和数量 2:得 试剂位置和总数
                                            //3:得 post信息
{


    if(T_tableexecute->modelOperate == 2 || T_tableexecute->modelOperate == 5)//get
    {
        if(order == 1)//get draer number
        {

            int rowAll = T_model_execute->rowCount();

            int E_drawer = 0;
            int E_drawer_previous = 0;
            int drawerAll = 0;

            for(int i = 0 ; i < rowAll; i++)
            {
                E_drawer = T_model_execute->data(T_model_execute->index(i,5)).toInt();

                if(E_drawer_previous != E_drawer)
                {
                    drawerAll++;
                    sheet_drawer[0] = drawerAll;
                    sheet_drawer[drawerAll] = E_drawer;
                    E_drawer_previous = E_drawer;
                }

            }

        }
        if(order == 2)//get position info
        {
            sheet_position[0] = T_model_execute->rowCount();

            for( int i = 0 ; i < sheet_position[0] ;i++)
            {
                sheet_position[i+1] = T_model_execute->data(T_model_execute->index(i,6)).toInt();
                qDebug()<<sheet_position[i+1];
            }

        }

    }



    return 0;
}

void Sheet_OperatePage::getWriter(int i,int model)//model 0:已摆放 1：未摆放 2：误取 3：误放
{
    QString error;
    if(model == 0){
            error = "已摆放";

    }else if(model == 1){

            error = "未摆放";

    }else if(model == 2){

            error = "误取";

    }else if(model == 3){

            error = "误放";

    }else if(model == 4){
            error = "错误";
    }

    T_model_execute->setData(T_model_execute->index(i,11),error);
    T_model_execute->submitAll();

}

void Sheet_OperatePage::showNeedData(int i, char order)//0:show current drawer agentia
                                                      // 1:show all drawer agentia
{
    if(order == 0)
    {
        T_model_execute->setFilter(QString("drawerNo like %1").arg(i));//筛选结果
        T_model_execute->sort(6, Qt::AscendingOrder);

        updateLabel(i);

    }
    if(order == 1)
    {
        sheetTableInit(0);//只初始化表格

        updateLabel(0);
    }


}

void Sheet_OperatePage::sendOrder_to_STM(int status)//status 0:success 2：query
{

    if(status == 0)
    {
        getRead(2);//获得试剂位置信息
        SCI_send(0);
    }

    if(status == 2)//2:查询 //0: 错误 1：未完成 2：正确
    {

       /**************查询指令******************/

//        T_tableexecute->error = SCI_send(1);
        T_tableexecute->error = 2;

        /**************************************/
// Alarm_No[0]错误    Alarm_No[1] 移动    Alarm_No[2]未成
        if(T_tableexecute->error == 0)//错误
        {
            QString printfInfo = "";
            QMessageBox qMbox;
            for(int i = 0;i < 32 ;i++)
            {
                if(Alarm_No[0][i] != 0)//写入错误操作
                    printfInfo += QString("%1,").arg(Alarm_No[0][i]);


            }
            if(Alarm_No[0][0] != 0)
            {
                qMbox.setText(QString("错误操作：%1").arg(printfInfo));
                qMbox.exec();
                InfoError(0,printfInfo);
            }


            ui->pBt_ignore->hide();
            ui->pBt_return->hide();

            InfoError(0,printfInfo);

       }
       else if(T_tableexecute->error == 2)//正确
       {
            SCI_send(2);

            for(int i = 0 ; i < sheet_position[0]; i++)
            {
                getWriter(i,0);
            }
            pBtStatus(1);
            ui->pBt_ignore->show();
            ui->pBt_return->show();
            InfoError(1,"操作正确");
       }
       else if(T_tableexecute->error == 1)//无错未放置
       {
            for(int i = 0 ; i < sheet_position[0]; i++)
            {
                getWriter(i,0);
            }
            int Nod = 0;
            for(int i = 0; i < 32;i++)
            {
                Nod = Alarm_No[2][i];
                if(Nod == 0)
                {

                    break;
                }
                else
                {
                    specialWrite(0,0,Alarm_No[2][i]);
                }

            }

            InfoError(2,"是否跳过");
            ui->pBt_ignore->show();
            ui->pBt_return->show();

       }

    }


}
void Sheet_OperatePage::specialWrite(int order,int error, int i) //0：未操作 1：错误 2：移动
{
    QSqlQuery query;
    QString err = "";
    if(error == 0)
    {
       err = "未操作";
    }
    else if(error == 1)
    {
        err = "错误";
    }
    else if(error == 2)
    {
        err = "移动";
    }
    if(order == 0)
    {
        query.exec(QString("update %3 set judgeAttitude='%1' where positionNo=%2").arg(err).arg(i).arg(T_tableexecute->T_tablesheet));

    }
}

void Sheet_OperatePage::pBtStatus(int status)//正常发送 1:调用下发指令 2：调用查询指令
{

    if(status == 1)
    {

        if(sheet_model < sheet_drawer[0])
        {
            sheet_model++;

            showNeedData(sheet_drawer[sheet_model],0);//show current drawer agentia info


            /*******指令下发*********************/

            sendOrder_to_STM(0);

            /*******************************/

            ui->pBt_OK->setText(QString("抽屉 %1 完成").arg(sheet_drawer[sheet_model]));
        }
        else if (sheet_model >= sheet_drawer[0])
        {


            showNeedData(1,1);
            ui->pBt_OK->setText(QString("点击退出"));
            ui->label_error->setText(QString("完成"));


            sendHttp();

        }
        else
        {
            sheet_model++;
        }

    }


    else if(status == 2)
    {
        /*******************/
        sendOrder_to_STM(2);
        /********************/
        sheet_status = 1;
    }
}

void Sheet_OperatePage::closeOperate()
{
    emit closePrevious();
    waitTaskInfo(100);
    this->close();

}

void Sheet_OperatePage::sendHttp()
{
    int rowAll = T_model_execute->rowCount();
    ui->pBt_OK->setEnabled(false);
    ui->pBt_return->setEnabled(false);
    ui->pBt_ignore->setEnabled(false);

    if(T_tableexecute->modelOperate == 2)
    {
        http_GAndP_sheetOperate->JsonForSend(7, T_tableexecute->T_tablesheet, rowAll);

    }
    else if(T_tableexecute->modelOperate == 5)
    {
        http_GAndP_sheetOperate->JsonForSend(10, T_tableexecute->T_tablesheet, rowAll);
    }
}

void Sheet_OperatePage::receivers_From_http()
{
    sheetTableInit(0);
    ui->pBt_OK->setEnabled(true);
    ui->pBt_return->setEnabled(true);
    ui->pBt_ignore->setEnabled(true);
    sheet_status = 3;//切换按钮功能 3：退出
}

void Sheet_OperatePage::delSheetInfo(int status,QString name ,int i)//0: 全部清除 1：选定删除内容
{
    QSqlQuery query;

    if(T_tableexecute->modelOperate == 2 || T_tableexecute->modelOperate == 5)
    {
        if(status == 0)
        {
            query.exec(QString("DELETE from %1").arg(name));
        }
        if(status == 1)
        {
            query.exec(QString("delete from %1 where rowid = %2").arg(name).arg(i));
        }
    }


}

int Sheet_OperatePage::SCI_send(int order) //0:下发 1：查询 2：完成
{
//    if(order == 0)
//    {

////        IntoDrawer(0x01);
//        int send_positionNo[32]={0};
//        for(int i=0;i<sheet_position[0];i++)
//        {
//            send_positionNo[i]=sheet_position[i+1];
//        }
//        unsigned char DataAct[8]={0};
//        SetDataAct(send_positionNo,sheet_position[0],DataAct);
//        if(wait4SetAct(0x01,DataAct)==-1)
//            qDebug("ERROR！");

//    }
//    else if(order == 1)
//    {
//        return wait4Alarm(0x01);
//    }
//    else if(order == 2)
//    {
//        wait4SendAct(0x01,0);

//    }
//    return true;
}

void Sheet_OperatePage::on_pBt_ignore_clicked()
{

    if (SCI_send(1))
    {
        SCI_send(2);
        pBtStatus(1);
    }
    else
    {
        InfoError(0,"错误纠正");
    }
    InfoError(1,"请继续操作");
}


void Sheet_OperatePage::InfoError(int num, QString error)//输出任务状态并执行未成功数据保存
//任务完成情况
{
    QPalette pe;
    if (num == 0)//红
    {
        pe.setColor(QPalette::WindowText,Qt::red);
    }
    else if (num == 1)//绿
    {
        pe.setColor(QPalette::WindowText,Qt::green);
    }
    else if (num == 2)//蓝
    {      
        pe.setColor(QPalette::WindowText,Qt::blue);
    }

    ui->label_error->setPalette(pe);
    ui->label_error->setText(error);
}
