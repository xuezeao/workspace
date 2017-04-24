#include "dg_execcheck.h"
#include "ui_dg_execcheck.h"

Dg_ExecCheck::Dg_ExecCheck(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dg_ExecCheck)
{
    ui->setupUi(this);

    t_exec = new QSqlTableModel(this);
    query = new QSqlQuery;
    netCommunication = new NetCommunication(this);
    threadSerialPort = new QThread;
    serialPort = new SerialPortControl;
    timer = new QTimer(this);
    agetiaInfo = new Save_AgentiaInfo;
    //变量初始化
    InitializeStruct();

    count = 0;
    lockStatus = -1;

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
    save_drawer.clear();
    save_position.clear();
    save_number.clear();
    save_previousErrorPosition.clear();
    save_warningInfo.clear();

    //正则表达
    QRegExp regx_bottleCapacity("[0-9]+$");//"[a-zA-Z0-9]+$" 长度7纯数字输入3.3格式
    QValidator *validator_bottleCapacity = new QRegExpValidator(regx_bottleCapacity,ui->lE_volum);
    ui->lE_volum->setValidator(validator_bottleCapacity);

#ifndef TEST
    this->showFullScreen();
#endif
    //      showMaximized();
    //      setWindowFlags(Qt::FramelessWindowHint);

    SetTitle("点验");
    //执行程序
    ProcessHandle();
}

Dg_ExecCheck::~Dg_ExecCheck()
{
    delete ui;
}


void Dg_ExecCheck::ProcessHandle()
{

    //显示界面
    ShowPage();
    //获取试剂抽屉
    GetDrawerAndPositionNum();
    //创建线程
    CreateAndStartThread();
    //打开串口
    CreateSerialPort();

    waitTime(500);
    if (isOpenSerialPort)
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


//T_AgentiaReturnExecute
void Dg_ExecCheck::ShowPage()
{
    t_exec->setTable("T_AgentiaExecute");
    t_exec->select();

    t_exec->setHeaderData(0, Qt::Horizontal, QObject::tr("编号"));
    t_exec->setHeaderData(1, Qt::Horizontal, QObject::tr("试剂名"));
    t_exec->setHeaderData(2, Qt::Horizontal, QObject::tr("状态"));
    t_exec->setHeaderData(3, Qt::Horizontal, QObject::tr("剩余容积"));
    t_exec->setHeaderData(5, Qt::Horizontal, QObject::tr("抽屉号"));
    t_exec->setHeaderData(6, Qt::Horizontal, QObject::tr("位置号"));

    t_exec->setEditStrategy(QSqlTableModel::OnManualSubmit);
    t_exec->sort(4, Qt::AscendingOrder);

    ui->TV_exec->setModel(t_exec);

    ui->TV_exec->setColumnHidden(4,true);
    ui->TV_exec->setColumnHidden(7,true);
    ui->TV_exec->setColumnHidden(8,true);
    ui->TV_exec->setColumnHidden(9,true);
    ui->TV_exec->setColumnHidden(10,true);

    ui->TV_exec->setSelectionMode(QAbstractItemView::NoSelection);
    ui->TV_exec->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TV_exec->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->TV_exec->sortByColumn(5, Qt::AscendingOrder);
}

//获取抽屉号、位置号、编号
void Dg_ExecCheck::GetDrawerAndPositionNum()
{
    int rowCount = t_exec->rowCount();

    int currentDrawer = -1;
    int currentPosition = -1;
    int currentNumber = -1;

    for (int i = 0; i < rowCount; i++)
    {
        currentDrawer = t_exec->data(t_exec->index(i, 5)).toInt();
        currentPosition = t_exec->data(t_exec->index(i, 6)).toInt();
        currentNumber = t_exec->data(t_exec->index(i, 0)).toInt();

        save_drawer.insert(i, currentDrawer);
        save_position.insert(i, currentPosition);
        save_number.insert(i, currentNumber);
    }
}

//显示下一条任务信息
void Dg_ExecCheck::ShowCurrentNeedExecuteTaskInfo(int i)
{
    QString num = "";
    if (i < save_number.size())
    {
        num = QString::number(save_number[i]);
    }
    else
    {
        num = "";
    }
    t_exec->setFilter(QString("id like '%%1%'").arg(num));//筛选结果
}

void Dg_ExecCheck::ClearSql()
{
    query->exec(QString("DELETE from T_AgentiaReturnExecute"));
}

//关闭指令
void Dg_ExecCheck::ClosePage()
{
    isClickReturnPB = false;
    t_exec->deleteLater();
    timer->deleteLater();
    netCommunication->deleteLater();
    threadSerialPort->deleteLater();//删除线程
    this->deleteLater();
}


void Dg_ExecCheck::on_pB_sure_clicked()
{
    if (count < save_drawer.size())
    {
        int volum_i = ui->lE_volum->text().toInt();
        QString unit = ui->CB_unit->currentText();
        QString volum = QString::number(volum_i);
        if (volum != "")
        {
            QString newVolum = volum + unit;

            t_exec->setData(t_exec->index(0, 3), newVolum);
            t_exec->submitAll();
        }
    }
}

void Dg_ExecCheck::on_CB_return_clicked()
{
    isQuitLoopRequestAgentiaStatus = true;
    isClickReturnPB = true;
    TextMessageShowContent("", "请关闭当前抽屉");

    if (count < save_drawer.size())
    {
        RequestLockStatus_Current(count);
        TextMessageShowContent("", "任务完成，正在执行上报，请稍候");
        UploadTask2Server();
    }
    if (count >= save_drawer.size())
    {
        emit QuitExecPage();
    }
    ClearSql();
}

void Dg_ExecCheck::on_pB_jump_clicked()
{
    isQuitLoopRequestAgentiaStatus = true;
    AllControl(jumpCurrentAct);
}

void Dg_ExecCheck::CreateAndStartThread()
{
    serialPort->moveToThread(threadSerialPort);
    threadSerialPort->start();
}

void Dg_ExecCheck::CreateSerialPort()
{
    emit OpenSerialPort();
}

void Dg_ExecCheck::ErrorMessage(int order)
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

void Dg_ExecCheck::HandleReply(int order)
{
    if (TASK_ERROR == order)
        AllControl(haveErrorHandle);
    else if (TASK_HAVENOTEXEC == order)
        AllControl(haveNotExecTask);
    else if (TASK_OVER == order)
        AllControl(havetaskOver);
}

void Dg_ExecCheck::ChangeStatus(QString content)
{
    QString info = t_exec->data(t_exec->index(0, 2)).toString();
    QString judge = "审批";

    if (info == "审批" )
    {
        ChangeCellContent(0, 2, judge+content);
    }
    else
    {
        ChangeCellContent(0, 2, content);
    }

    t_exec->submitAll();
    qDebug()<< t_exec->data(t_exec->index(0, 2)).toString();
}


void Dg_ExecCheck::TaskSend()
{
    int DID = save_drawer[count];
    int send_ActNum = 1;
    send_positionNo_G[0] = save_position[count];

    emit TaskAct_Send(DID, send_ActNum);
}

void Dg_ExecCheck::NextTask()
{
    GetError();//异常上报
    count++;

    if (count < save_drawer.size())
    {
        if (save_drawer[count - 1] != save_drawer[count])
        {
            TextMessageShowContent("", QString("请关闭抽屉：%1 ，以便继续进行操作")\
                                   .arg(save_drawer[count - 1]));
            //等待柜子关闭
        }
    }
    else
    {
        TextMessageShowContent("", QString("请关闭抽屉：%1 ，以便继续进行操作")\
                               .arg(save_drawer[count - 1]));
        RequestLockStatus_Current(count - 1);
    }


    ShowCurrentNeedExecuteTaskInfo(count);

    if ( count < save_drawer.size())
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
void Dg_ExecCheck::RequestLockStatus_Current(int i)
{
    emit GetLockStatus(save_drawer[i]);
    waitTaskInfo(500);
    if (lockStatus == DRAWEROPEN)
        RequestLockStatus_Current(i);
}

//已执行任务上报
void Dg_ExecCheck::UploadTask2Server()
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
        decision = t_exec->data(t_exec->index(i, 2)).toString();

        if ("审批完成" == decision)
        {
            taskId = t_exec->data(t_exec->index(i, 9)).toInt();
            json_str = netCommunication->PackageJson_taskComplete(userId, taskId);

            if (netCommunication->PostHttp("taskComplete", json_str, 2))
            {
                if (UnpackageJson_Task(netCommunication->ServerReply))
                {
                    ChangeCellContent(i, 2, "上传成功");
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
            int positionId = t_exec->data(t_exec->index(i, 10)).toInt();
            int agetiaId = t_exec->data(t_exec->index(i, 9)).toInt();
            QString dose = t_exec->data(t_exec->index(i, 4)).toString();

            json_str = netCommunication->PackageJson_giveBack(userId, agetiaId,\
                                                              dose, positionId);
            //上传成功
            if (netCommunication->PostHttp("giveBack", json_str, 2))
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
                agetiaInfo->getC_judgeAttitude = "归还";
                agetiaInfo->getC_agentiaId = agetiaId;
                agetiaInfo->getC_position = positionId;
                agetiaInfo->getC_dose = dose;

                WriteSQL_NetworkError();
            }
        }
    }
    TextMessageShowContent("", "请退出");
}
//json信息解析
bool Dg_ExecCheck::UnpackageJson_Task(QJsonDocument str)
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

void Dg_ExecCheck::ChangeCellContent(int row, int column, QString content)
{
    t_exec->setData(t_exec->index(row, column), content);
    t_exec->submitAll();
}

void Dg_ExecCheck::TextMessageShowContent(QString colorType, QString content)
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

void Dg_ExecCheck::WriteSQL_NetworkError()
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

void Dg_ExecCheck::InitializeStruct()
{
    agetiaInfo->getC_agentiaId = 0;
    agetiaInfo->getC_bottleCapacity = "";
    agetiaInfo->getC_dose = "";
    agetiaInfo->getC_expireDate = "";
    agetiaInfo->getC_judgeAttitude = "";
    agetiaInfo->getC_position = 0;
}

void Dg_ExecCheck::UploadWarning(QString json_str, QString warningContent, int model)
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
            int taskId = t_exec->data(t_exec->index(0, 9)).toInt();
            agetiaInfo->getC_agentiaId = taskId;
        }
        agetiaInfo->getC_bottleCapacity = json_str;
        WriteSQL_NetworkError();
    }
}

//禁用抽屉和位置
void Dg_ExecCheck::UploadDisableDrawerAndPosition(int model)
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
void Dg_ExecCheck::GetError()
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

    save_warningInfo.clear();
}

void Dg_ExecCheck::AllControl(int order)
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
                break;
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
        ui->textB_message->setText(QStringLiteral("连接异常，上报服务器中..."));
        QString str = netCommunication->PackageJson_warningLog(userId, QString("抽屉异常，编号：%1")\
                                                             .arg(save_drawer[count]));
        UploadWarning(str, QString("抽屉异常，编号：%1").arg(save_drawer[count]), DrawerError);
        UploadDisableDrawerAndPosition(DrawerError);

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

void Dg_ExecCheck::isSaveError(int goal)
{
    if (!save_previousErrorPosition.empty())
    {
        for (int i = 0; i < save_previousErrorPosition.size(); i++)
        {
            if (goal == save_previousErrorPosition[i])
            {
                save_warningInfo.append(goal);//预备上传内容
                break;
            }
        }
    }
}

//接受锁的状态
void Dg_ExecCheck::LockStatusSlots(int order)
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


void Dg_ExecCheck::waitTime(int time)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<time)
    {
        QCoreApplication::processEvents();
    }
}

void Dg_ExecCheck::SetTitle(QString name)
{
    ui->lB_title->setText(name);
}


