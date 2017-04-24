#include "dg_exectakeout.h"
#include "ui_dg_exectakeout.h"
#include "common/globalvariable.h"
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRegExp>
#include <QValidator>
#include <QElapsedTimer>
#include <QCoreApplication>

Dg_ExecTakeOut::Dg_ExecTakeOut(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dg_ExecTakeOut)
{
    ui->setupUi(this);
    t_exec = new QSqlTableModel(this);
    query = new QSqlQuery;
    netCommunication = new NetCommunication(this);
    threadSerialPort = new QThread;
    serialPort = new SerialPortControl;
    timer = new QTimer(this);
    agetiaInfo = new Save_AgentiaInfo;

#ifndef TEST
    this->showFullScreen();
#endif
    //      showMaximized();
    //      setWindowFlags(Qt::FramelessWindowHint);

    //变量初始化
    InitializeStruct();
    isOpenSerialPort = false;
    count = 0;
    lockStatus = -1;
    preDrawerNo = -1;

    save_drawer.clear();
    save_number.clear();
    save_position.clear();
    save_previousErrorPosition.clear();
    save_warningInfo.clear();
    save_outstripTimer.clear();
    save_needDel.clear();

    s_drawerColumn = 6;
    s_positioinColumn = 7;
    s_judgeStatusColumn = 2;
    s_nameColumn = 1;
    s_agetiaIdColumn = 10;
    s_positionIdColumn = 11;


    connect(this, SIGNAL(TaskAct_Send(int,int)), serialPort,\
            SLOT(HandleFlow(int,int)));
    connect(this, SIGNAL(GetLockStatus(int)), serialPort, \
            SLOT(ReturnLockStatus(int)));
    connect(this, SIGNAL(OpenSerialPort()), serialPort, \
            SLOT(CreateSerialPort()));
    connect(this, SIGNAL(QuitExecPage()), serialPort, \
            SLOT(DestroyProcess()));

    connect(serialPort, SIGNAL(Task_Error(int)), \
            this, SLOT(ErrorMessage(int)));
    connect(serialPort, SIGNAL(destroyed()), \
            this, SLOT(ClosePage()));
    connect(serialPort, SIGNAL(ReplyCurrentAgentiaStatus(int)),\
            this, SLOT(HandleReply(int)));
    connect(serialPort, SIGNAL(ReturnLock(int)), this, SLOT(LockStatusSlots(int)));



    //执行程序
    ProcessHandle();
}

Dg_ExecTakeOut::~Dg_ExecTakeOut()
{
    delete ui;
}

void Dg_ExecTakeOut::ProcessHandle()
{
    //显示界面
    ShowPage();
    //获取试剂抽屉
    GetDrawer();
    //创建线程
    CreateAndStartThread();
    //打开串口
    CreateSerialPort();

    waitTime(500);
    QSqlQuery query;
    query.exec(QString("select * from T_AgentiaExecute"));
    query.last();
    int rowCount = query.at() + 1;
    if (isOpenSerialPort && rowCount > 0)
    {
        //加载第一条任务信息
        ShowCurrentNeedExecuteTaskInfo(0);
        //下发第一条任务
        TaskSend();
    }
    else
    {
        QMessageBox::warning(this, "警告", "串口打开失败");
        this->deleteLater();
    }
}

//T_AgentiaExecute
void Dg_ExecTakeOut::ShowPage()
{
    t_exec->setTable("T_AgentiaExecute");
    t_exec->select();

    t_exec->setHeaderData(0, Qt::Horizontal, QObject::tr("编号"));
    t_exec->setHeaderData(1, Qt::Horizontal, QObject::tr("试剂名"));
    t_exec->setHeaderData(2, Qt::Horizontal, QObject::tr("状态"));
    t_exec->setHeaderData(5, Qt::Horizontal, QObject::tr("剩余容积"));
    t_exec->setHeaderData(6, Qt::Horizontal, QObject::tr("抽屉号"));
    t_exec->setHeaderData(7, Qt::Horizontal, QObject::tr("位置号"));

    t_exec->setEditStrategy(QSqlTableModel::OnManualSubmit);
    t_exec->sort(4, Qt::AscendingOrder);

    ui->TV_exec->setModel(t_exec);

    ui->TV_exec->setColumnHidden(3,true);
    ui->TV_exec->setColumnHidden(4,true);
    ui->TV_exec->setColumnHidden(8,true);
    ui->TV_exec->setColumnHidden(9,true);
    ui->TV_exec->setColumnHidden(10,true);
    ui->TV_exec->setColumnHidden(11,true);
    ui->TV_exec->setColumnHidden(12,true);
    ui->TV_exec->setColumnHidden(13,true);
    ui->TV_exec->setColumnHidden(14,true);
    ui->TV_exec->setColumnHidden(15,true);


    ui->TV_exec->setSelectionMode(QAbstractItemView::NoSelection);
    ui->TV_exec->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TV_exec->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //降序
    ui->TV_exec->sortByColumn(6, Qt::AscendingOrder);
}

//获取抽屉号
void Dg_ExecTakeOut::GetDrawer()
{
    save_drawer.clear();
    int rowCount = t_exec->rowCount();

    int currentDrawer = -1;
    int preDrawerNo = -1;

    for (int i = 0; i < rowCount; i++)
    {
        currentDrawer = t_exec->data(t_exec->index(i, s_drawerColumn)).toInt();

        if (preDrawerNo != currentDrawer)
        {
            preDrawerNo = currentDrawer;
            save_drawer.append(currentDrawer);
        }
    }
}

void Dg_ExecTakeOut::GetPositionNum()
{
    int rowCount = t_exec->rowCount();
    int currentPosition = -1;
    save_position.clear();

    for (int i = 0; i < rowCount; i++)
    {
        currentPosition = t_exec->data(t_exec->index(i, s_positioinColumn)).toInt();
        save_position.insert(i, currentPosition);
    }
}

//显示下一条任务信息
void Dg_ExecTakeOut::ShowCurrentNeedExecuteTaskInfo(int i)
{
    QString num = "";
    if (i < save_drawer.size())
        num = QString::number(save_drawer[i]);
    else
        num = "";

    t_exec->setFilter(QString("drawerNo like '%%1%'").arg(num));//筛选结果
}

//关闭指令
void Dg_ExecTakeOut::ClosePage()
{
    isClickReturnPB = false;
    t_exec->deleteLater();
    timer->deleteLater();
    netCommunication->deleteLater();
    threadSerialPort->deleteLater();//删除线程
    this->deleteLater();
}

void Dg_ExecTakeOut::on_CB_return_clicked()
{
    isClickReturnPB = true;
    DelSQL();
    TextMessageShowContent("", "请关闭当前抽屉");
    if (count < save_drawer.size())
    {
        RequestLockStatus_Current(count);
        TextMessageShowContent("", "任务完成，正在执行上报，请稍候");
        UploadTask2Server();

        if (isQuitLoopRequestAgentiaStatus == true)
        {
            emit QuitExecPage();
        }
    }
    //flag1
    else if (count >= save_drawer.size())
    {
        emit QuitExecPage();
    }

    TextMessageShowContent("", "正在退出····");
    isQuitLoopRequestAgentiaStatus = true;
}

void Dg_ExecTakeOut::DelSQL()
{
    t_exec->select();

    int rowCount = t_exec->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        QString decision = t_exec->data(t_exec->index(i, s_judgeStatusColumn)).toString();
        if (decision == "完成" || decision == "审批完成")
        {
            QString name = t_exec->data(t_exec->index(i, s_nameColumn)).toString();
            save_needDel.append(name);
        }
    }

    for (int j = 0; j < save_needDel.size(); j++)
    {
        query->prepare(QString("update T_AgentiaSaving set judgeAttitude='%1',userDisplayName='%2' \
                               where agentiaName='%3'")
               .arg("借出").arg(userName).arg(j));
        query->exec();
    }
    save_needDel.clear();
}


void Dg_ExecTakeOut::on_pB_jump_clicked()
{
    isQuitLoopRequestAgentiaStatus = true;
    AllControl(jumpCurrentAct);
}

void Dg_ExecTakeOut::CreateAndStartThread()
{
    serialPort->moveToThread(threadSerialPort);
    threadSerialPort->start();
}

void Dg_ExecTakeOut::CreateSerialPort()
{
    emit OpenSerialPort();
}

void Dg_ExecTakeOut::ErrorMessage(int order)
{
    switch (order) {
    case ACT_INTODRAWER_FAIL:
    {
        emit GetLockStatus(save_drawer[count]);
        ui->textB_message->setText(QStringLiteral("开锁失败"));
        break;
    }
    case ACT_CREATESERIALPORT_SUCCESS:
    {
        ui->textB_message->setText(QStringLiteral("串口打开"));
        AllControl(serialPortOpenSuccess);
        break;
    }
    case ACT_CREATESERIALPORT_FAIL:
    {
        ui->textB_message->setText(QStringLiteral("串口打开失败"));
        AllControl(serialPortOpenFail);
        break;
    }
    case ACT_SENDTASK_FAIL:
    {
        ui->textB_message->setText(QStringLiteral("下发指令失败"));
        AllControl(actSendFail);
        break;
    }
    case ACT_WORKVOER_FAIL:
    {
        ui->textB_message->setText(QStringLiteral("清除失败，正在重试"));
        break;
    }
    case ACT_TASK_OK:
    {
        AllControl(nextTask);
        break;
    }
    case TASK_NOTREPLY:
    {
        TextMessageShowContent("", "下位机异常");
        waitTaskInfo(1000);
        AllControl(actSendFail);
        break;
    }
    default:
        break;
    }
}

void Dg_ExecTakeOut::HandleReply(int order)
{
    if (TASK_ERROR == order)
        AllControl(haveErrorHandle);
    else if (TASK_HAVENOTEXEC == order)
        AllControl(haveNotExecTask);
    else if (TASK_OVER == order)
        AllControl(havetaskOver);
}

void Dg_ExecTakeOut::ChangeStatus(QString content)
{
    int rowCount = t_exec->rowCount();
    if (content == "跳过")
    {
        if (0 != Alarm_No[2][0])
        {
            for (int i = 0; i < 64; i++)
            {
                if (0 != Alarm_No[2][i])
                {
                    for(int i = 0; i < rowCount; i++)
                    {
                        int position = t_exec->data(t_exec->index(i, s_positioinColumn)).toInt();
                        if (position == Alarm_No[2][i])
                        {
                            QString info = t_exec->data(t_exec->index(i, s_judgeStatusColumn)).toString();

                            if (info == "审批" )
                            {
                                t_exec->setData(t_exec->index(i, s_judgeStatusColumn), "审批跳过");
                            }
                            else
                            {
                                t_exec->setData(t_exec->index(i, s_judgeStatusColumn), "跳过");
                            }

                            t_exec->submitAll();
                        }
                    }
                }
            }
        }
    }
    else if (content == "完成")
    {
        for (int i = 0; i < rowCount; i++)
        {
            ChangeCellContent(i, s_judgeStatusColumn, "完成");
        }
    }
    t_exec->submitAll();
}

void Dg_ExecTakeOut::NextTask()
{
    GetError();//异常上报
    TextMessageShowContent("", QString("请关闭抽屉：%1 ，以便继续进行操作")\
                           .arg(save_drawer[count]));
    //等待柜子关闭
    RequestLockStatus_Current(count);

    count++;
    ShowCurrentNeedExecuteTaskInfo(count);

    if (count < (save_drawer.size()))
    {
        waitTaskInfo(500);
        TaskSend();
    }
    else
    {
        TextMessageShowContent("", "任务完成，正在执行上报，请稍候");
        UploadTask2Server();
    }
}

//请求当前锁的状态，直至抽屉关闭
void Dg_ExecTakeOut::RequestLockStatus_Current(int i)
{
    emit GetLockStatus(save_drawer[i]);
    waitTaskInfo(500);
    if (lockStatus == DRAWEROPEN)
        RequestLockStatus_Current(i);
}


void Dg_ExecTakeOut::TaskSend()
{
    //获取位置
    GetPositionNum();
    for (int i = 0; i < 64; i++)
    {
        send_positionNo_G[0] = 0;
    }
    for (int i = 0; i < save_position.size(); i++)
    {
        send_positionNo_G[i] = save_position[i];
    }

    int DID = save_drawer[count];
    emit TaskAct_Send(DID, save_position.size());
}

//接受锁的状态
void Dg_ExecTakeOut::LockStatusSlots(int order)
{
    if (DRAWERCLOSE == order)
    {
        lockStatus = DRAWERCLOSE;
    }
    else if (DRAWEROPEN == order)
    {
        lockStatus = DRAWEROPEN;
    }
}

//已执行任务上报
void Dg_ExecTakeOut::UploadTask2Server()
{
    QString json_str = "";
    int taskId = -1;
    QString decision = "";

    t_exec->select();

    int rowCount = t_exec->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        bool isSuccess = false;
        InitializeStruct();
        decision = t_exec->data(t_exec->index(i, s_judgeStatusColumn)).toString();

        if ("审批完成" == decision)
        {
            taskId = t_exec->data(t_exec->index(i, s_agetiaIdColumn)).toInt();
            json_str = netCommunication->PackageJson_taskComplete(userId, taskId);

            if (netCommunication->PostHttp("taskComplete", json_str, 2))
            {
                if (UnpackageJson_Task(netCommunication->ServerReply))
                {
                    ChangeCellContent(i, s_judgeStatusColumn, "上传成功");
                    isSuccess = true;
                }
            }

            if (isSuccess)
            {
                TextMessageShowContent("blue", "上传失败，已存储");
                agetiaInfo->getC_judgeAttitude = "任务";
                agetiaInfo->getC_agentiaId = taskId;

                WriteSQL_NetworkError();
            }

        }

        else if ("完成" == decision)
        {
            int positionId = t_exec->data(t_exec->index(i, s_positionIdColumn)).toInt();
            int agetiaId = t_exec->data(t_exec->index(i, s_agetiaIdColumn)).toInt();

            json_str = netCommunication->PackageJson_takeOut(userId, agetiaId,\
                                                             positionId);
            //上传成功
            if (netCommunication->PostHttp("takeOut", json_str, 2))
            {
                if (UnpackageJson_Task(netCommunication->ServerReply))
                {
                    ChangeCellContent(i, 2, "上传成功");
                    isSuccess = true;
                }
            }

            if (!isSuccess)
            {
                TextMessageShowContent("blue", "上传失败，已存储");
                agetiaInfo->getC_judgeAttitude = "取用";
                agetiaInfo->getC_agentiaId = agetiaId;
                agetiaInfo->getC_position = positionId;

                WriteSQL_NetworkError();
            }
        }
    }
    TextMessageShowContent("", "请退出");
}

//json信息解析
bool Dg_ExecTakeOut::UnpackageJson_Task(QJsonDocument str)
{
    QJsonValue s_json[1] = {0};
    bool s_success = false;
    QJsonObject analyze_Z = str.object();

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    if (s_success)
    {
        return true;
    }

    return false;
}

void Dg_ExecTakeOut::ChangeCellContent(int row, int column, QString content)
{
    t_exec->setData(t_exec->index(row, column), content);
    t_exec->submitAll();
}

void Dg_ExecTakeOut::TextMessageShowContent(QString colorType, QString content)
{
    if (colorType == "green")
        ui->textB_message->setTextColor(Qt::green);
    else if (colorType == "red")
        ui->textB_message->setTextColor(Qt::red);
    else if (colorType == "blue")
        ui->textB_message->setTextColor(Qt::blue);
    else if (colorType == "")
        ui->textB_message->setTextColor(Qt::black);

    ui->textB_message->setText(content);
}

void Dg_ExecTakeOut::WriteSQL_NetworkError()
{
    query->exec("select * from T_WaitPostInfo");
    query->last();
    int rowCount = query->at() + 1;

    query->prepare("insert into T_WaitPostInfo (id,userId,agentiaId,\
                  positionId,dose,expireDate,bottleCapacity,judgeAttitude) \
                    values (?,?,?,?,?,?,?,?)");

    query->addBindValue(rowCount + 1);
    query->addBindValue(userId);
    query->addBindValue(agetiaInfo->getC_agentiaId);
    query->addBindValue(agetiaInfo->getC_position);
    query->addBindValue(agetiaInfo->getC_dose);
    query->addBindValue(agetiaInfo->getC_expireDate);
    query->addBindValue(agetiaInfo->getC_bottleCapacity);
    query->addBindValue(agetiaInfo->getC_judgeAttitude);
    query->exec();
}

void Dg_ExecTakeOut::InitializeStruct()
{
    agetiaInfo->getC_agentiaId = 0;
    agetiaInfo->getC_bottleCapacity = "";
    agetiaInfo->getC_dose = "";
    agetiaInfo->getC_expireDate = "";
    agetiaInfo->getC_judgeAttitude = "";
    agetiaInfo->getC_position = 0;
}

void Dg_ExecTakeOut::UploadWarning(QString json_str, QString warningContent, int model)
{
    bool isSuccess = false;

    InitializeStruct();
    if (netCommunication->PostHttp("warningLog", json_str, 2))
    {
        if (UnpackageJson_Task(netCommunication->ServerReply))
        {
            TextMessageShowContent("green", warningContent);
            isSuccess = true;
        }
    }

    if (!isSuccess)
    {
        agetiaInfo->getC_judgeAttitude = "报警";

        if (model == OperateError)
        {
            int taskId = t_exec->data(t_exec->index(0, s_agetiaIdColumn)).toInt();
            agetiaInfo->getC_agentiaId = taskId;
        }
        agetiaInfo->getC_bottleCapacity = json_str;
        WriteSQL_NetworkError();
    }
}

//禁用抽屉和位置
void Dg_ExecTakeOut::UploadDisableDrawerAndPosition(int model)
{
    bool isSuccess = false;
    QString str = "";
    InitializeStruct();

    if (model == DrawerError)
    {
        str = netCommunication->PackageJson_disablePosition(userId, CABINETNO, save_drawer[count]);
        agetiaInfo->getC_agentiaId = save_drawer[count];
        agetiaInfo->getC_judgeAttitude = "禁用抽屉";
    }
    else if (model == PositionError)
    {
        str = netCommunication->PackageJson_disablePosition(userId, CABINETNO, save_drawer[count],\
                                                            save_position[count]);
        agetiaInfo->getC_agentiaId = save_drawer[count];
        agetiaInfo->getC_position = save_position[count];
        agetiaInfo->getC_judgeAttitude = "禁用位置";
    }

    if (netCommunication->PostHttp("disablePosition", str, 2))
    {
        if (UnpackageJson_Task(netCommunication->ServerReply))
        {
            isSuccess =  true;
            TextMessageShowContent("", "此抽屉异常，禁用状态");
        }
    }

    if (!isSuccess)
    {
        agetiaInfo->getC_bottleCapacity = str;
        WriteSQL_NetworkError();
    }
}

//上报异常操作
void Dg_ExecTakeOut::GetError()
{
    InitializeStruct();
    bool isSuccess = false;
    QString description = QString("用户： %1 , 有如下异常操作 抽屉号： %2 ,位置如下：").arg(userId).arg(save_drawer[count]);
    if (!save_warningInfo.empty())
    {
        for (int i = 0; i < save_warningInfo.size(); i++)
        {
            if (0 != save_warningInfo[i])
            {
                description += QString::number(save_warningInfo[i])+", ";
            }
            else
            {
                break;
            }
        }

        QString str = netCommunication->PackageJson_warningLog(userId, description);
        if (netCommunication->PostHttp("warningLog", str, 2))
        {
            if (UnpackageJson_Task(netCommunication->ServerReply))
            {
                isSuccess = true;
            }
            else
            {
                isSuccess = false;
            }
        }
        if (!isSuccess)
        {
            agetiaInfo->getC_judgeAttitude = "报警";
            agetiaInfo->getC_bottleCapacity = str;
            WriteSQL_NetworkError();
        }
    }
    save_outstripTimer.clear();
    save_warningInfo.clear();
}

void Dg_ExecTakeOut::AllControl(int order)
{
    switch (order) {
    case serialPortOpenSuccess:
    {
        isOpenSerialPort = true;
        break;
    }
    case serialPortOpenFail:
    {
        isOpenSerialPort = false;
        break;
    }
    case jumpCurrentAct:
    {
        TextMessageShowContent("", "跳过上一条指令");
        ChangeStatus("跳过");
        break;
    }
    case haveErrorHandle:
    {
        if (0 != Alarm_No[0][0])
        {
            QString warning = "有错误操作，记录位置：";
            for (int i = 0; i < 64; i++)
            {
                if (0 != Alarm_No[0][i])
                {
                    warning += QString::number(Alarm_No[0][i])+", ";
                    isSaveError(Alarm_No[0][i]);
                }
                else
                {
                    break;
                }
            }
            warning = warning + "    请纠正";
            ui->textB_message->setText(warning);
        }

        save_previousErrorPosition.clear();
        for (int i = 0; i < 64; i++)
        {
            if (0 != Alarm_No[0][i])
            {
                save_previousErrorPosition[i] = Alarm_No[0][i];
            }
            else
            {
                break;
            }
        }

        break;
    }
    case haveNotExecTask:
    {
        if (0 != Alarm_No[2][0])
        {
            QString warning = "有未完成，位置为： ";
            for (int i = 0; i < 64; i++)
            {
                if (0 != Alarm_No[2][i])
                {
                    warning += QString::number(Alarm_No[2][i])+", ";
                }
                else
                {
                    break;
                }
            }
            warning = warning + "   可点击按钮跳过";
            TextMessageShowContent("", warning);
        }

        break;
    }
    case havetaskOver:
    {
        TextMessageShowContent("", "操作无误");
        ChangeStatus("完成");
        break;
    }
    case actSendFail:
    {
        if (isClickReturnPB)
            break;
        ui->textB_message->setText(QStringLiteral("连接异常，上报服务器中..."));
        QString str = netCommunication->PackageJson_warningLog(userId, QString("抽屉异常，编号：%1")\
                                                             .arg(save_drawer[count]));
        UploadWarning(str, QString("抽屉异常，编号：%1").arg(save_drawer[count]), DrawerError);
        UploadDisableDrawerAndPosition(DrawerError);
        AllControl(nextTask);
        break;
    }
    case nextTask:
    {
        if (!isClickReturnPB)
            NextTask();
        break;
    }

    default:
        break;
    }
}

void Dg_ExecTakeOut::isSaveError(int goal)
{
    int value = 0;


    if (!save_previousErrorPosition.empty())
    {
        for (int i = 0; i < save_previousErrorPosition.size(); i++)
        {
            if (goal == save_previousErrorPosition[i])
            {
                value = save_outstripTimer[goal];
                if (value >= 5)
                {
                    if (save_warningInfo.indexOf(goal) == -1)
                    {
                        save_warningInfo.append(goal);//预备上传内容
                    }
                    save_outstripTimer[goal] = 0;
                }
                save_outstripTimer[goal] = value + 1;
                break;
            }
        }
    }
}

void Dg_ExecTakeOut::waitTime(int time)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<time)
    {
        QCoreApplication::processEvents();
    }
}

