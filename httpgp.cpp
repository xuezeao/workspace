#include "httpgp.h"
#include "global_Vailable.h"

HttpGP::HttpGP(QObject *parent) :
    QObject(parent)
{
    user = new User;
    http_info = new Http_info;

    accessManager = new QNetworkAccessManager(this);
    msgBox = new QMessageBox;




    connect(accessManager,SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)), Qt::QueuedConnection);

}

void HttpGP::JuageOperatorStatus(int order)
//判断是否完成任务
//-2:网络错误 -1：任务失败 0：任务完成
//1：获取在位试剂 2：获取试剂类型 3：获取待归还试剂 4：获取地址信息
//5：分配位置 6：入柜完成上报 7：取完成上报 8：还上报 9：报废 10：替换 11：登入 12:点验
//13：报警 14：任务列表 15：网络任务完成上报
//16:获取柜子信息
{
    taskHandleCount++;
    if (0 == order)
    {
        EmitSignal(0, http_info->model_json);
    }
    else if (-2 == order)
    {
        SaveDateNotSendInfo(http_info->http_modelChoice, http_info->tableName);
    }
    else if (-1 == order)
    {
        EmitSignal(-1, http_info->model_json);
    }
    else
    {
        if (taskHandleCount < http_info->allTaskCount)
        {
            PackageJson(http_info->model_json, http_info->tableName, taskHandleCount);
        }
        else
        {
            EmitSignal(0, http_info->model_json);
        }
    }


}

void HttpGP::JsonForSend(int model_json, QString T_tableName, int allTaskCount)
{

    HttpInit();

    http_info->model_json = model_json;
    http_info->tableName = T_tableName;
    http_info->allTaskCount = allTaskCount;

    if (11 == model_json)//登入模式
    {
        taskHandleCount = allTaskCount;//存储用户信息
    }
    else
    {
        taskHandleCount = 0;
    }

    PackageJson(model_json, T_tableName, taskHandleCount);
}

void HttpGP::HttpInit(void)
{
    http_info->http_modelChoice = 0;
    http_info->model_json = 0;
    http_info->tableNo = 0;
    http_info->tableName = "";
    http_info->taskRole = 0;
    http_info->allTaskCount = 0;
}

void HttpGP::GetHttp(int order)
//1：机柜信息 2：心跳包  3：服务器时间
{

    QString address;
    switch (order) {
    case 1:
    {
        http_info->http_modelChoice = 0;
        http_info->model_json = 0;
        address = QString("initialInfo/%1").arg(CABINETNO);
        break;
    }
    case 2:
    {
        http_info->http_modelChoice = 17;
        http_info->model_json = 17;
        address = "heartBeating";
        break;
    }
    case 3:
    {
        http_info->http_modelChoice = 18;
        http_info->model_json = 18;
        address = "serverTime";
        break;
    }
    default:
        break;
    }

    QNetworkRequest *request=new QNetworkRequest();
//    request->setUrl(QUrl(QString("http://localhost:3000/arm/initialInfo/%1").arg(CABINETNO)));
    request->setUrl(QUrl(QString("http://121.43.159.215:3000/arm/%1").arg(address)));
    accessManager->get(*request);//通过发送数据，返回值保存在reply指针里.
}



void HttpGP::PostHttp(int postId_NO, QString postStr)
//0:测试地址 1：获取在位试剂 2：获取试剂类型 4：获取待归还试剂 5:请求空闲位置
//6:入柜完成 7：取完成  8：还完成  9：替换完成 10：报废完成 11：登入 12：点验 13：报警信息 14：任务列表
//15：网络任务完成上报
{


    QString address = "";

    switch (postId_NO){
    case 0 :{
        address="postTest";//测试地址
        break;
    }
    case 1 :{
        address = "availableAgentiaList";//获取在位试剂列表
        http_info->http_modelChoice = 1;
        break;
    }
    case 2 :{
        address = "agentiaTypeList";//获取试剂类型列表
        http_info->http_modelChoice = 2;
        break;
    }
    case 4 :{
        address = "occupiedAgentiaList";//获取待归还试剂
        http_info->http_modelChoice = 4;
        break;
    }
    case 5 :{
        address = "allocPosition";//入柜位置申请
        http_info->http_modelChoice = 5;
        break;
    }
    case 6 :{
        address = "putIn";//入柜完成
        http_info->http_modelChoice = 6;
        break;
    }
    case 7 :{
        address = "takeOut";//取 完成
        http_info->http_modelChoice = 7;
        break;
    }
    case 8 :{
        address = "giveBack";//还 完成
        http_info->http_modelChoice = 8;
        break;
    }
    case 9 :{
        address = "replace";//替换 完成
        http_info->http_modelChoice = 9;
        break;
    }
    case 10 :{
        address = "scrap";//报废 完成
        http_info->http_modelChoice = 10;
        break;
    }
    case 11 :{
        address = "login";//登入
        http_info->http_modelChoice = 11;
        break;
    }
    case 12 :{
        address = "check";//点验
        http_info->http_modelChoice = 12;
        break;
    }
    case 13:{
        address = "warningLog";//报警
        http_info->http_modelChoice = 13;
        break;
    }
    case 14:{
        address = "taskList";//获取任务列表
        http_info->http_modelChoice = 14;
        break;
    }
    case 15:{
        address = "taskComplete";//申请任务完成 上报
        http_info->http_modelChoice = 15;
        break;
    }
    default:
        break;
    }

    QNetworkRequest *request=new QNetworkRequest();
//    request->setUrl(QUrl(QString("http://localhost:3000/arm/%1").arg(address)));
    request->setUrl(QUrl(QString("http://121.43.159.215:3000/arm/%1").arg(address)));
    request->setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QByteArray postData = postStr.toUtf8();//翻译
    accessManager->post(*request, postData);

    qDebug() << QDateTime::currentDateTime() << "send";
}

void HttpGP::finished(QNetworkReply *reply)
{

    qDebug()<<" current thread id : -----------" << QThread::currentThreadId();
     qDebug() << QDateTime::currentDateTime() << "received";

     if (reply->error() == QNetworkReply::NoError)
     {
         QTextCodec *codec = QTextCodec::codecForName("utf-8");
         QString all = codec->toUnicode(reply->readAll());
         QJsonDocument all_info = QJsonDocument::fromJson(all.toUtf8());

         qDebug() << QDateTime::currentDateTime() << "starXIE";

         int currentRole = UnpackageJson(all_info, http_info->http_modelChoice);//解析
         qDebug() << QDateTime::currentDateTime() << "JIEXIE";
         JuageOperatorStatus(currentRole);//判断
     }
     else
     {
         qDebug() << "http error here finished";
//         JuageOperatorStatus(-2);//判断
         JuageOperatorStatus(-1);
     }

     reply->deleteLater();//释放reply

}

int HttpGP::UnpackageJson(QJsonDocument str, int t)
// 0:获取机柜信息 1：获取在位试剂 2：获取试剂类型 4：获取待归还试剂
//5：分配位置 6：入柜完成上报 7：取完成上报 8：还上报 9：替换 10：报废 11：登入 12:点验 13：报警14:任务列表
//15:网络任务完成上报 17:心跳包 18:服务器时间
{


    qDebug() << " current thread Id : UP "<<QThread::currentThreadId();


    QString    s_str[11]    = {0};//save string
    int        s_int[11]    = {0};//save int value
    int        s_allInfoNum = 0;//save all info number
    QJsonValue s_json[11]   = {0};//save jsonvalue
    QJsonArray s_JA;                //savef jsonArray
    QJsonObject analyze_Z = str.object();;
    QJsonObject analyze_C;
    /*****存储信息******/
    int  s_positionNo = 0;
    int  s_drawerNO   = 0;
    int  s_positionId = 0;
    int  s_taskCount  = 0;
    int  s_taskId     = 0;
    bool s_success    = false;

    QString month = "";
    QString year = "";
    QString day = "";
    QString time = "";

    QString newMonth = "";
    QString newYear = "";
    QString newDay = "";
    QString newTime = "";
    /***************************/
    s_taskCount = taskHandleCount + 1;
    /*****************************/
    if (t == 18)
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            s_json[1] = analyze_Z["serverTime"].toString();
            s_str[1] = s_json[1].toString();
            qDebug()<<s_str[1]<<"111111111111111111";
            return 0;
        }
    }
    else if (t == 17)
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();
        qDebug()<<s_success<<QThread::currentThreadId()<<"2222222222222222222222222";
        return 0;
    }
    else if (t == 0)
    {
        query.exec(QString("DELETE from T_CabinetInfo"));//机柜信息

        s_json[0] = analyze_Z["cabinetName"].toString();
        s_str[0]  = s_json[0].toString();

        s_json[1] = analyze_Z["groupId"].toInt();
        s_int[1]  = s_json[1].toInt();
        user->user_groupId = s_int[1];

        s_json[2] = analyze_Z["groupName"].toString();
        s_str[2]  = s_json[2].toString();

        s_json[3] = analyze_Z["drawerAmount"].toInt();
        s_int[3]  = s_json[3].toInt();

        s_allInfoNum = s_int[3];

        query.prepare("insert into T_CabinetInfo (cabinetName,groupId,groupName,drawerAmount) values (?,?,?,?)");
        query.addBindValue(s_str[0]);
        query.addBindValue(s_int[1]);
        query.addBindValue(s_str[2]);
        query.addBindValue(s_int[3]);
        query.exec();

        s_JA = analyze_Z["drawerList"].toArray();

        for (int i = 0; i < s_allInfoNum; i++)
        {
            analyze_C = s_JA[i].toObject();

            s_json[0] = analyze_C["drawerNo"].toInt();
            s_int[0]  = s_json[0].toInt();

            s_json[1] = analyze_C["drawerName"].toString();
            s_str[1]  = s_json[1].toString();

            s_json[2] = analyze_C["drawerSize"].toString();
            s_str[2]  = s_json[2].toString();

            s_json[3] = analyze_C["positionAmount"].toInt();
            s_int[3]  = s_json[3].toInt();

            s_json[4] = analyze_C["attribte"].toInt();
            s_int[4]  = s_json[4].toInt();

            query.prepare("insert into T_DrawerList (drawerNo,drawerName,drawerSize,positionAmount,attribute) values (?,?,?,?,?)");
            query.addBindValue(s_int[0]);
            query.addBindValue(s_str[1]);
            query.addBindValue(s_str[2]);
            query.addBindValue(s_int[3]);
            query.addBindValue(s_int[4]);
            query.exec();

        }
        qDebug() << "机柜消息解析完毕";
        return 0;

    }
    else if (t == 1)//获取在位试剂列表
    {
        query.exec(QString("DELETE from T_AgentiaSaving"));//获取在位试剂列表

        s_json[0] = analyze_Z["amount"].toInt();
        s_int[0]  = s_json[0].toInt();

        s_allInfoNum = s_int[0];

        s_JA = analyze_Z["agentiaList"].toArray();

        for (int i = 0; i < s_allInfoNum; i++)
        {
            analyze_C = s_JA[i].toObject();

            s_json[0] = analyze_C["agentiaName"].toString();
            s_str[0]  = s_json[0].toString();

            s_json[1] = analyze_C["bottleCapacity"].toString();
            s_str[1]  = s_json[1].toString();

            s_json[2] = analyze_C["dose"].toString();
            s_str[2]  = s_json[2].toString();

            s_json[3] = analyze_C["expiryDate"].toString();
            s_str[3]  = s_json[3].toString();
            year = s_str[3].section("-",0,0);
            month = s_str[3].section("-",1,1);
            day = s_str[3].section("-",2,2);
            day = day.section("T",0,0);
            time = QString("%1.%2.%3").arg(year).arg(month).arg(day);

            s_json[4] = analyze_C["drawerNo"].toInt();
            s_int[4]  = s_json[4].toInt();

            s_json[5] = analyze_C["positionNo"].toInt();
            s_int[5]  = s_json[5].toInt();

            s_json[6] = analyze_C["agentiaId"].toInt();
            s_int[6]  = s_json[6].toInt();

            s_json[7] = analyze_C["positionId"].toInt();
            s_int[7]  = s_json[7].toInt();

            s_json[8] = analyze_C["attribute"].toInt();
            s_int[8]  = s_json[8].toInt();

            query.prepare("insert into T_AgentiaSaving (id,checkBox,agentiaName,bottleCapacity,dose,\
                                                            drawerNo,positionNo,expireDate,attribute,agentiaId,positionId,judgeAttitude\
                                                            ) values (?,?,?,?,?,?,?,?,?,?,?,?)");
            query.addBindValue(i+1);
            query.addBindValue(QString("未选择"));
            query.addBindValue(s_str[0]);
            query.addBindValue(s_str[1]);
            query.addBindValue(s_str[2]);
            query.addBindValue(s_int[4]);
            query.addBindValue(s_int[5]);
            query.addBindValue(time);
            query.addBindValue(s_int[8]);
            query.addBindValue(s_int[6]);
            query.addBindValue(s_int[7]);
            query.addBindValue(QString("未归还"));
            query.exec();

       }

       qDebug() << "获取在位试剂列表";
       return 0;
    }
    else if (t == 2)//获取试剂类型
    {
        query.exec(QString("DELETE from T_AgentiaTypeList"));//获取试剂类型

        s_json[0] = analyze_Z["amount"].toInt();
        s_int[0]  = s_json[0].toInt();

        s_allInfoNum = s_int[0];

        s_JA = analyze_Z["agentiaTypeList"].toArray();

        for (int i = 0; i < s_allInfoNum; i++)
        {
            analyze_C = s_JA[i].toObject();

            s_json[0] = analyze_C["agentiaName"].toString();
            s_str[0]  = s_json[0].toString();

            s_json[1] = analyze_C["abbreviation"].toString();
            s_str[1]  = s_json[1].toString();

            s_json[2] = analyze_C["chemicalFormula"].toString();
            s_str[2]  = s_json[2].toString();

            s_json[3] = analyze_C["specification"].toString();
            s_str[3]  = s_json[3].toString();

            s_json[4] = analyze_C["factory"].toString();
            s_str[4]  = s_json[4].toString();

            s_json[5] = analyze_C["itemNo"].toString();
            s_str[5]  = s_json[5].toString();

            s_json[6] = analyze_C["attribute"].toInt();
            s_int[6]  = s_json[6].toInt();

            s_json[7] = analyze_C["agentiaTypeId"].toInt();
            s_int[7]  = s_json[7].toInt();

            query.prepare("insert into T_AgentiaTypeList (id,checkBox,agentiaName,abbreviation,chemicalFormula,\
                                                            specification,factory,attribute,agentiaTypeId,itemNo\
                                                            ) values (?,?,?,?,?,?,?,?,?,?)");
            query.addBindValue(i+1);
            query.addBindValue(QString("未选择"));
            query.addBindValue(s_str[0]);
            query.addBindValue(s_str[1]);
            query.addBindValue(s_str[2]);
            query.addBindValue(s_str[3]);
            query.addBindValue(s_str[4]);
            query.addBindValue(s_int[6]);
            query.addBindValue(s_int[7]);
            query.addBindValue(s_str[5]);

            query.exec();

       }

       qDebug() << "获取试剂类型";
       return 0;

    }
    else if (t == 4)//获取待归还试剂
    {
        query.exec(QString("DELETE from T_AgentiaWaitSaving"));//带归还试剂信息

        s_json[0] = analyze_Z["amount"].toInt();
        s_int[0]  = s_json[0].toInt();

        s_allInfoNum = s_int[0];

        s_JA = analyze_Z["agentiaList"].toArray();

        for (int i = 0; i < s_allInfoNum; i++)
        {
            analyze_C = s_JA[i].toObject();

            s_json[0] = analyze_C["agentiaName"].toString();
            s_str[0]  = s_json[0].toString();

            s_json[1] = analyze_C["bottleCapacity"].toString();
            s_str[1]  = s_json[1].toString();

            s_json[2] = analyze_C["dose"].toString();
            s_str[2]  = s_json[2].toString();

            s_json[3] = analyze_C["expiryDate"].toString();
            s_str[3]  = s_json[3].toString();
            year = s_str[3].section("-",0,0);
            month = s_str[3].section("-",1,1);
            day = s_str[3].section("-",2,2);
            day = day.section("T",0,0);
            time = QString("%1.%2.%3").arg(year).arg(month).arg(day);

            s_json[4] = analyze_C["drawerNo"].toInt();
            s_int[4]  = s_json[4].toInt();

            s_json[5] = analyze_C["positionNo"].toInt();
            s_int[5]  = s_json[5].toInt();

            s_json[6] = analyze_C["agentiaId"].toInt();
            s_int[6]  = s_json[6].toInt();

            s_json[7] = analyze_C["positionId"].toInt();
            s_int[7]  = s_json[7].toInt();

            query.prepare("insert into T_AgentiaWaitSaving (id,checkBox,agentiaName,bottleCapacity,dose,\
                                                            expireDate,drawerNo,positionNo,agentiaId,positionId,judgeAttitude\
                                                            ) values (?,?,?,?,?,?,?,?,?,?,?)");
            query.addBindValue(i+1);
            query.addBindValue(1);
            query.addBindValue(s_str[0]);
            query.addBindValue(s_str[1]);
            query.addBindValue(s_str[2]);
            query.addBindValue(time);
            query.addBindValue(s_int[4]);
            query.addBindValue(s_int[5]);
            query.addBindValue(s_int[6]);
            query.addBindValue(s_int[7]);
            query.addBindValue(QString("未归还"));
            query.exec();

       }

       qDebug() << "带归还试剂列表解析完毕";
       return 0;

    }
    else if (t == 5)//入柜申请分配位置
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            s_json[0]    = analyze_Z["positionNo"].toInt();
            s_positionNo = s_json[0].toInt();

            s_json[1]    = analyze_Z["drawerNo"].toInt();
            s_drawerNO   = s_json[1].toInt();

            s_json[2]    = analyze_Z["positionId"].toInt();
            s_positionId = s_json[2].toInt();
            user->positionId_allocPosition = s_positionId;

            query.exec(QString("update %1 set positionId='%2',drawerNo='%3',positionNo='%4',\
                               judgeAttitude='%5' where id=%6")
                       .arg(http_info->tableName).arg(s_positionId).arg(s_drawerNO).arg(s_positionNo)
                       .arg(QString("已分配位置")).arg(s_taskCount));


        }
        else
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("缺乏相应位置")).arg(s_taskCount));
            qDebug() << "error--request positionId";
        }

        qDebug() << "position unpackage success";
        return t;

    }
    else if (t == 6)//入柜完成
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传成功")).arg(s_taskCount));
        }
        else
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传失败")).arg(s_taskCount));

            qDebug() << "error--send putin ok";
        }

        qDebug() << "PutIn operator unpackage success";
        return t;
    }
    else if (t == 7)//取 完成
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传成功")).arg(s_taskCount));
        }
        else
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传失败")).arg(s_taskCount));
            qDebug() << "error--send Get ok";
        }

        qDebug() << "get agentia unpackage success";
        return t;
    }
    else if (t == 8)//还 完成
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传成功")).arg(s_taskCount));
        }
        else
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传失败")).arg(s_taskCount));

            qDebug() << "error--send give back ok";
        }

        qDebug() << "give back agentia unpackage success";
        return t;
    }
    else if (t == 9)//替换完成
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传成功")).arg(s_taskCount));
        }
        else
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传失败")).arg(s_taskCount));

            qDebug() << "error--send change ok";
        }

        qDebug() << "chenge agentia unpackage success";
        return t;
    }
    else if (t == 10)//报废
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传成功")).arg(s_taskCount));
        }
        else
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传失败")).arg(s_taskCount));

            qDebug() << "error--send remove ok";
        }

        qDebug() << "scrape unpackage success";
        return t;
    }
    else if (t == 11)//登入
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            s_json[0] = analyze_Z["userId"].toInt();
            user->user_id = s_json[0].toInt();

            s_json[1] = analyze_Z["role"].toInt();
            user->user_role = s_json[1].toInt();

            query.exec(QString("DELETE from T_UserInfo"));//初始化表格

            query.prepare("insert into T_UserInfo (userId,role) values (?,?)");
            query.addBindValue(user->user_id);
            query.addBindValue(user->user_role);
            query.exec();

            return t;
        }
        else
        {
            return -1;
            qDebug() << "error--send enter";
        }
    }
    else if (t == 12)//点验
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传成功")).arg(s_taskCount));
        }
        else
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传失败")).arg(s_taskCount));

            qDebug() << "error--send check ok";
        }

        qDebug() << "check unpackage success";
        return t;
    }
    else if (t == 13)//报错
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            return t;
        }
        else
        {
            return -1;
            qDebug() << "error--send check ok";
        }
    }
    else if (15 == t)//task完成 上报
    {
        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传成功")).arg(s_taskCount));
        }
        else
        {
            query.exec(QString("update %1 set judgeAttitude='%2' where id=%3")
                       .arg(http_info->tableName).arg(QString("上传失败")).arg(s_taskCount));

            qDebug() << "error--send check ok";
        }

        qDebug() << "tasklist unpackage success";
        return http_info->model_json;
    }
    else if (t == 14)//获取任务列表
    {

        s_json[0] = analyze_Z["success"].toBool();
        s_success = s_json[0].toBool();

        if (s_success)
        {
            int  s_taskStatus = 0;
            int  s_taskType   = 0;
            /*******************初始化表格***********************/

            query.exec(QString("DELETE from Task_sheet"));//任务列表
            query.exec(QString("DELETE from T_Task_PutIn"));//入柜
            query.exec(QString("DELETE from T_AgentiaWaitExecute"));//归还
            query.exec(QString("DELETE from T_AgentiaReplace"));//替换
            query.exec(QString("DELETE from T_AgentiaScrap"));//报废
            query.exec(QString("DELETE from T_AgentiaCheck"));//点验
            query.exec(QString("DELETE from T_AgentiaExecute"));//取

            /*******************************************/

            s_json[0] = analyze_Z["amount"].toInt();
            s_allInfoNum = s_json[0].toInt();

            s_JA = analyze_Z["taskList"].toArray();
            for (int i = 0; i < s_allInfoNum; i++)
            {
                analyze_C = s_JA[i].toObject();

                s_json[0] = analyze_C["taskType"].toInt();
                s_int[0]  = s_json[0].toInt();
                s_taskType= s_int[0];

                s_json[1] = analyze_C["taskStatus"].toInt();
                s_int[1]  = s_json[1].toInt();
                s_taskStatus = s_int[1];

                time = "0";
                s_json[2] = analyze_C["expiryDate"].toString();
                s_str[2]  = s_json[2].toString();
                year = s_str[2].section("-",0,0);
                month = s_str[2].section("-",1,1);
                day = s_str[2].section("-",2,2);
                day = day.section("T",0,0);
                time = QString("%1.%2.%3").arg(year).arg(month).arg(day);

                s_json[3] = analyze_C["agentiaName"].toString();
                s_str[3]  = s_json[3].toString();

                s_json[4] = analyze_C["dose"].toString();
                s_str[4]  = s_json[4].toString();

                s_json[5] = analyze_C["drawerNo"].toInt();
                s_int[5]  = s_json[5].toInt();

                s_json[6] = analyze_C["positionNo"].toInt();
                s_int[6]  = s_json[6].toInt();

                s_json[7] = analyze_C["cabinetNo"].toString();
                s_str[7]  = s_json[7].toString();

                s_json[8] = analyze_C["newAgentia_dose"].toString();
                s_str[8]  = s_json[8].toString();

                newTime = "0";
                s_json[9] = analyze_C["newAgentia_expiryDate"].toString();
                s_str[9]  = s_json[9].toString();
                newYear = s_str[9].section("-",0,0);
                newMonth = s_str[9].section("-",1,1);
                newDay = s_str[9].section("-",2,2);
                newDay = day.section("T",0,0);
                newTime = QString("%1.%2.%3").arg(newYear).arg(newMonth).arg(newDay);

                s_json[10]= analyze_C["taskId"].toInt();
                s_int[10] = s_json[10].toInt();
                s_taskId = s_int[10];

                /***************加入任务列表*************/
                query.prepare("insert into Task_sheet (id,taskId,taskType,taskStatus,\
                              expiryDate,agentiaName,dose,positionNo,drawerNo,\
                              cabinetNo,newAgentia_dose,newAgentia_expiryDate) values (?,?,?,?,?,?,\
                                                                                       ?,?,?,?,?,?)");
                query.addBindValue(i+1);
                query.addBindValue(s_int[10]);
                query.addBindValue(s_int[0]);
                query.addBindValue(s_int[1]);
                query.addBindValue(time);
                query.addBindValue(s_str[3]);
                query.addBindValue(s_str[4]);
                query.addBindValue(s_int[5]);
                query.addBindValue(s_int[6]);
                query.addBindValue(s_str[7]);
                query.addBindValue(s_str[8]);
                query.addBindValue(newTime);
                query.exec();

                /********************************将任务分别加入各自执行表格中去**************************/
                if (1 == s_taskStatus)//审核通过
                {
                    if (0 == s_taskType)//入柜
                    {
                        query.prepare("insert into T_Task_PutIn (id,checkBox,changeInfo,agentiaName,judgeAttitude,\
                                      bottleCapacity,dose,drawerSize,expireDate,roleStatus,\
                                      attribute,agentiaTypeId,chemicalFormula,specification,\
                                      factory,itemNo,positionId,drawerNo,positionNo) values (?,?,?,?,?,\
                                                                                             ?,?,?,?,?,\
                                                                                             ?,?,?,?,\
                                                                                             ?,?,?,?,?)");
                        query.addBindValue(i+1);
                        query.addBindValue(QString("未选择"));
                        query.addBindValue(QString("1"));
                        query.addBindValue(s_str[3]);
                        query.addBindValue(QString("已审批"));
                        query.addBindValue(QString(""));
                        query.addBindValue(s_str[4]);
                        query.addBindValue(QString(""));
                        query.addBindValue(time);
                        query.addBindValue(0);
                        query.addBindValue(3);
                        query.addBindValue(s_taskType);
                        query.addBindValue(QString(""));
                        query.addBindValue(QString(""));
                        query.addBindValue(QString(""));
                        query.addBindValue(QString(""));
                        query.addBindValue(s_taskId);
                        query.addBindValue(s_int[5]);
                        query.addBindValue(s_int[6]);

                        query.exec();
                    }
                    else if (1 == s_taskType)//取用
                    {
                        query.prepare("insert into T_AgentiaExecute (id,checkBox,agentiaName,bottleCapacity,\
                                      dose,drawerNo,positionNo,expireDate,attribute,\
                                      agentiaId,positionId,judgeAttitude) values (?,?,?,?,\
                                                                                  ?,?,?,?,?,\
                                                                                  ?,?,?)");
                        query.addBindValue(i+1);
                        query.addBindValue(QString("未选择"));
                        query.addBindValue(s_str[3]);
                        query.addBindValue(QString(""));
                        query.addBindValue(s_str[4]);
                        query.addBindValue(s_int[5]);
                        query.addBindValue(s_int[6]);
                        query.addBindValue(time);
                        query.addBindValue(3);
                        query.addBindValue(s_taskType);
                        query.addBindValue(s_taskId);
                        query.addBindValue(QString("已审批"));

                        query.exec();
                    }
                    else if (2 == s_taskType)//归还
                    {
                        query.prepare("insert into T_AgentiaWaitExecute (id,checkBox,agentiaName,bottleCapacity,\
                                      dose,expireDate,returnA,changeA,desertA,\
                                      drawerNo,positionNo,attribute,agentiaId,positionId,judgeAttitude) values (?,?,?,?,\
                                                                                  ?,?,?,?,?,?,\
                                                                                  ?,?,?,?,?)");
                        query.addBindValue(i+1);
                        query.addBindValue(QString("未选择"));
                        query.addBindValue(s_str[3]);
                        query.addBindValue(QString(""));
                        query.addBindValue(s_str[4]);
                        query.addBindValue(time);
                        query.addBindValue(QString("1"));
                        query.addBindValue(QString("1"));
                        query.addBindValue(QString("1"));
                        query.addBindValue(s_int[5]);
                        query.addBindValue(s_int[6]);
                        query.addBindValue(3);
                        query.addBindValue(s_taskType);
                        query.addBindValue(s_taskId);
                        query.addBindValue(QString("已审批"));

                        query.exec();
                    }
                    else if (3 == s_taskType)//替换
                    {
                        query.prepare("insert into T_AgentiaReplace (id,checkBox,changeInfo,agentiaName,bottleCapacity,\
                                      dose,newdose,expireDate,drawerNo,positionNo,attribute,\
                                      agentiaId,positionId,judgeAttitude) values (?,?,?,?,?,\
                                                                                  ?,?,?,?,?,?,\
                                                                                  ?,?,?)");
                        query.addBindValue(i+1);
                        query.addBindValue(QString("未选择"));
                        query.addBindValue(QString(""));
                        query.addBindValue(s_str[3]);
                        query.addBindValue(QString(""));
                        query.addBindValue(s_str[4]);
                        query.addBindValue(s_str[8]);
                        query.addBindValue(newTime);
                        query.addBindValue(s_int[5]);
                        query.addBindValue(s_int[6]);
                        query.addBindValue(3);
                        query.addBindValue(s_taskType);
                        query.addBindValue(s_taskId);
                        query.addBindValue(QString("已审批"));

                        query.exec();
                    }
                    else if (4 == s_taskType)//报废
                    {
                        query.prepare("insert into T_AgentiaScrap (id,checkBox,agentiaName,bottleCapacity,\
                                      dose,drawerNo,positionNo,expireDate,attribute,\
                                      agentiaId,positionId,judgeAttitude) values (?,?,?,?,\
                                                                                  ?,?,?,?,?,\
                                                                                  ?,?,?)");
                        query.addBindValue(i+1);
                        query.addBindValue(QString("未选择"));
                        query.addBindValue(s_str[3]);
                        query.addBindValue(QString(""));
                        query.addBindValue(s_str[4]);
                        query.addBindValue(s_int[5]);
                        query.addBindValue(s_int[6]);
                        query.addBindValue(time);
                        query.addBindValue(3);
                        query.addBindValue(s_taskType);
                        query.addBindValue(s_taskId);
                        query.addBindValue(QString("已审批"));

                        query.exec();
                    }
                    else if (5 == s_taskType)//点验
                    {
                        query.prepare("insert into T_AgentiaCheck (id,checkBox,changeInfo,agentiaName,bottleCapacity,\
                                      dose,newdose,expireDate,drawerNo,positionNo,attribute,\
                                      agentiaId,positionId,judgeAttitude) values (?,?,?,?,?,\
                                                                                  ?,?,?,?,?,?,\
                                                                                  ?,?,?)");
                        query.addBindValue(i+1);
                        query.addBindValue(QString("未选择"));
                        query.addBindValue(QString(""));
                        query.addBindValue(s_str[3]);
                        query.addBindValue(QString(""));
                        query.addBindValue(s_str[4]);
                        query.addBindValue(s_str[8]);
                        query.addBindValue(newTime);
                        query.addBindValue(s_int[5]);
                        query.addBindValue(s_int[6]);
                        query.addBindValue(3);
                        query.addBindValue(s_taskType);
                        query.addBindValue(s_taskId);
                        query.addBindValue(QString("已审批"));

                        query.exec();
                    }
                }
            }
            return 0;
        }
        else
        {
            return -1;
        }
    }
}


void HttpGP::PackageJson(int model_json, QString T_tableName, int T_tableNo)
//封装JSON信息
//0:获取机柜信息 1：获取在位试剂 2：获取试剂类型 4：获取待归还试剂  5:请求空闲位置
//6:入柜完成 7：取完成  8：还完成  9：替换完成 10：报废完成 11：登入 12：点验 13：报警信息 14：任务列表 15：网络任务完成上报
{

    qDebug() << " current thread id: " <<QThread::currentThreadId();

    QJsonObject json_Ok;
    QJsonObject json_Two;
    QJsonDocument document;
    QByteArray byte_array;

    QSqlQuery query;
    int stash_J_Int[10];
    QString stash_J_QString[10];
/************/
    user->user_id=1;//暂时使用
    qDebug() << QDateTime::currentDateTime() << "next task";
/**************/

    if (1 == model_json)//1：获取在位试剂
    {
        json_Ok.insert("cabinetNo",QString(CABINETNO));

        document.setObject(json_Ok);
        byte_array=document.toJson(QJsonDocument::Compact);
        QString json_str(byte_array);

        PostHttp(1,json_str);//发送http
    }
    else if (2 == model_json)//获取试剂类型
    {
        json_Ok.insert("type",99);
        json_Ok.insert("groupId",user->user_groupId);

        document.setObject(json_Ok);
        byte_array=document.toJson(QJsonDocument::Compact);
        QString json_str(byte_array);
        PostHttp(2,json_str);//发送http
    }
    else if (4 == model_json)
    {
        json_Ok.insert("cabinetNo",QString(CABINETNO));
        json_Ok.insert("userId",user->user_id);

        document.setObject(json_Ok);
        byte_array=document.toJson(QJsonDocument::Compact);
        QString json_str(byte_array);

        PostHttp(4,json_str);//发送http
    }
    else if (5 == model_json)//分配位置
    {
        query.exec(QString("select * from %1").arg(T_tableName));//从id=rownum中选中所有属性 '*' /也可指定 'name'
        query.seek(T_tableNo);

        stash_J_QString[0] = query.value(7).toString();//drawerSize
        stash_J_Int[1] = query.value(10).toInt();//attribut
        stash_J_QString[2] = query.value(4).toString();//judgeAttitude

        if (("已分配位置" == stash_J_QString[2]) || ("已审批" == stash_J_QString[2]))
        {
            JuageOperatorStatus(model_json);
        }
        else
        {
            json_Ok.insert("cabinetNo",QString(CABINETNO));//生成JSON
            json_Ok.insert("drawerSize",stash_J_QString[0]);
            json_Ok.insert("attribute",stash_J_Int[1]);//

            document.setObject(json_Ok);
            byte_array=document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);

            PostHttp(5,json_str);//发送http
        }
    }
    else if (6 == model_json)//入柜完成上报
    {
        query.exec(QString("select * from %1").arg(T_tableName));//从id=rownum中选中所有属性 '*' /也可指定 'name'
        query.seek(T_tableNo);

        stash_J_Int[0] = query.value(11).toInt();//agentiaTypeId
        stash_J_Int[1] = query.value(16).toInt();//PositionId
        stash_J_Int[2] = query.value(10).toInt();//attribute
        stash_J_QString[0] = query.value(5).toString();//bottleCapacity
        stash_J_QString[1] = query.value(6).toString();//dose
        stash_J_QString[2] = query.value(8).toString();//expireDate
        stash_J_QString[3] = query.value(4).toString();

        if(stash_J_QString[3] == "正确操作")
        {

            if (ALLOWOPERATE == stash_J_Int[2])
            {
                PackageJsonForTask(stash_J_Int[1]);
                return ;
            }

            json_Ok.insert("userId",user->user_id);//生成JSON
            json_Ok.insert("agentiaTypeId",stash_J_Int[0]);
            json_Ok.insert("bottleCapacity",stash_J_QString[0]);
            json_Ok.insert("dose",stash_J_QString[1]);
            json_Ok.insert("positionId",stash_J_Int[1]);
            json_Ok.insert("expiryDate",stash_J_QString[2]);

            document.setObject(json_Ok);
            byte_array=document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);

            PostHttp(6,json_str);//发送http
        }
        else
        {
            JuageOperatorStatus(model_json);
        }
    }
    else if (7 == model_json)//取 完成上报
    {
        query.exec(QString("select * from %1").arg(T_tableName));
        query.seek(T_tableNo);

        stash_J_Int[0] = query.value(9).toInt();//agentiaId
        stash_J_Int[1] = query.value(10).toInt();//positionId
        stash_J_Int[2] = query.value(8).toInt();//attribute
        stash_J_QString[0] = query.value(11).toString();//judgeAttitude

        if (stash_J_QString[0] == "已摆放")
        {
            if (ALLOWOPERATE == stash_J_Int[2])
            {
                PackageJsonForTask(stash_J_Int[1]);
                return ;
            }

            json_Ok.insert("userId",user->user_id);
            json_Ok.insert("agentiaId",stash_J_Int[0]);
            json_Ok.insert("positionId",stash_J_Int[1]);

            document.setObject(json_Ok);
            byte_array=document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);

            PostHttp(7,json_str);

        }
        else
        {
            JuageOperatorStatus(model_json);
        }
    }
    else if (8 == model_json)//归还
    {
        query.exec(QString("select * from %1").arg(T_tableName));
        query.seek(T_tableNo);

        stash_J_Int[0] = query.value(12).toInt();//agentiaId
        stash_J_Int[1] = query.value(13).toInt();//positionId
        stash_J_Int[2] = query.value(11).toInt();//attribute
        stash_J_QString[0] = query.value(13).toString();//judgeAttitude
        stash_J_QString[1] = query.value(4).toString();

        if(stash_J_QString[0] == "正确操作")
        {
            if (ALLOWOPERATE == stash_J_Int[2])
            {
                PackageJsonForTask(stash_J_Int[1]);
                return ;
            }

            json_Ok.insert("userId",user->user_id);
            json_Ok.insert("agentiaId",stash_J_Int[0]);
            json_Ok.insert("dose",stash_J_QString[1]);
            json_Ok.insert("positionId",stash_J_Int[1]);

            document.setObject(json_Ok);
            byte_array=document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);

            PostHttp(8,json_str);

        }
        else if (stash_J_QString[0] == "报废操作")
        {
            json_Ok.insert("userId",user->user_id);
            json_Ok.insert("agentiaId",stash_J_Int[0]);
            json_Ok.insert("positionId",stash_J_Int[1]);

            document.setObject(json_Ok);
            byte_array = document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);
            PostHttp(10,json_str);
        }
        else
        {
            JuageOperatorStatus(model_json);//未摆放
        }
    }
    else if (9 == model_json)//替换
    {
        query.exec(QString("select * from %1").arg(T_tableName));
        query.seek(T_tableNo);

       /**********/
        stash_J_Int[0] = query.value(11).toInt();//agentiaId
        stash_J_Int[1] = query.value(12).toInt();//positionId
        stash_J_Int[2] = query.value(10).toInt();//attribute
        stash_J_QString[0] = query.value(13).toString();//judgeAttitude
        stash_J_QString[1] = query.value(6).toString();//newDose
        stash_J_QString[2] = query.value(7).toString();//expireDate

        if (stash_J_QString[0] == "正确操作")
        {
            if (ALLOWOPERATE == stash_J_Int[2])
            {
                PackageJsonForTask(stash_J_Int[1]);
                return ;
            }

            json_Two.insert("dose",stash_J_QString[1]);
            json_Two.insert("expireDate",stash_J_QString[2]);

            json_Ok.insert("userId",user->user_id);
            json_Ok.insert("agentiaId",stash_J_Int[0]);
            json_Ok.insert("positionId",stash_J_Int[1]);
            json_Ok.insert("newAgentia",json_Two);


            document.setObject(json_Ok);
            byte_array = document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);
            PostHttp(9,json_str);

        }
        else if (stash_J_QString[0] == "报废操作")
        {
            json_Ok.insert("userId",user->user_id);
            json_Ok.insert("agentiaId",stash_J_Int[0]);
            json_Ok.insert("positionId",stash_J_Int[1]);

            document.setObject(json_Ok);
            byte_array = document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);
            PostHttp(10,json_str);

        }
        else if (stash_J_QString[0] == "替换操作")
        {
            json_Two.insert("dose",stash_J_QString[1]);
            json_Two.insert("expireDate",stash_J_QString[2]);

            json_Ok.insert("userId",user->user_id);
            json_Ok.insert("agentiaId",stash_J_Int[0]);
            json_Ok.insert("positionId",stash_J_Int[1]);
            json_Ok.insert("newAgentia",json_Two);


            document.setObject(json_Ok);
            byte_array = document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);
            PostHttp(9,json_str);
        }
        else
        {
            JuageOperatorStatus(model_json);//未摆放
        }
    }
    else if (10 == model_json)//报废
    {
        query.exec(QString("select * from %1").arg(T_tableName));
        query.seek(T_tableNo);
       /**********/
        stash_J_Int[0] = query.value(9).toInt();//agentiaId
        stash_J_Int[1] = query.value(10).toInt();//positionId
        stash_J_Int[2] = query.value(8).toInt();//attribute
        stash_J_QString[0] = query.value(11).toString();//judgeAttitude

        if (stash_J_QString[0] == "已摆放")
        {
            if (ALLOWOPERATE == stash_J_Int[2])
            {
                PackageJsonForTask(stash_J_Int[1]);
                return ;
            }

            json_Ok.insert("userId",user->user_id);
            json_Ok.insert("agentiaId",stash_J_Int[0]);
            json_Ok.insert("positionId",stash_J_Int[1]);

            document.setObject(json_Ok);
            byte_array=document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);

            PostHttp(10,json_str);
        }
        else
        {
            JuageOperatorStatus(model_json);//未摆放
        }

    }
    else if (11 == model_json)//登入
    {
        if (T_tableNo == 0)
        {
            user->userName = T_tableName;
        }
        else if (T_tableNo == 1)
        {
            stash_J_QString[0] = T_tableName;

            json_Ok.insert("username",user->userName);
            json_Ok.insert("password",stash_J_QString[0]);
            json_Ok.insert("cabinetNo",QString(CABINETNO));

            document.setObject(json_Ok);
            byte_array=document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);

            PostHttp(11,json_str);//发送http
        }
    }
    else if (12 == model_json)//点验
    {
        int errorType = 1;//默认是替换

        query.exec(QString("select * from %1").arg(T_tableName));
        query.seek(T_tableNo);
       /**********/
        stash_J_Int[0] = query.value(11).toInt();//agentiaId
        stash_J_Int[1] = query.value(12).toInt();//positionId
        stash_J_Int[2] = query.value(10).toInt();//attribute
        stash_J_QString[0] = query.value(13).toString();//judgeAttitude
        stash_J_QString[1] = query.value(6).toString();//newDose
        stash_J_QString[2] = query.value(7).toString();//expireDate

        if ((stash_J_QString[0] == "正确操作") || (stash_J_QString[0] == "报废操作") || (stash_J_QString[0] == "替换操作"))
        {

            if (ALLOWOPERATE == stash_J_Int[2])
            {
                PackageJsonForTask(stash_J_Int[1]);
                return ;
            }

            json_Ok.insert("userId",user->user_id);
            json_Ok.insert("agentiaId",stash_J_Int[0]);
            json_Ok.insert("positionId",stash_J_Int[1]);

            if (stash_J_QString[0] == "报废操作")
            {
                errorType = 2;
                stash_J_QString[1] = "";
                qDebug()<<"错误操作";
            }
            else if (stash_J_QString[0] == "正确操作")
            {
                errorType = 0;
                stash_J_QString[1] = "";
                qDebug()<<"正确操作";
            }

            json_Ok.insert("errorType",errorType);
            json_Ok.insert("correctDose",stash_J_QString[1]);

            document.setObject(json_Ok);
            byte_array = document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);
            PostHttp(12,json_str);

        }
        else
        {
            JuageOperatorStatus(model_json);//未摆放
        }
    }
    else if (14 == model_json)
    {
        json_Ok.insert("userId",user->user_id);
        json_Ok.insert("cabinetNo",QString(CABINETNO));

        document.setObject(json_Ok);
        byte_array = document.toJson(QJsonDocument::Compact);
        QString json_str(byte_array);
        PostHttp(14,json_str);
    }

}

void HttpGP::PackageJsonForTask(int taskId)
{
    QJsonObject json_Ok;
    QJsonDocument document;
    QByteArray byte_array;

    json_Ok.insert("userId",user->user_id);
    json_Ok.insert("taskId",taskId);

    document.setObject(json_Ok);
    byte_array=document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);

    qDebug() << "task任务";

    PostHttp(15,json_str);
}

void HttpGP::AgentiaUpdate(int order)
//更新常用数据 0：机柜信息 1：在位试剂 2：试剂类型 4：待归还试剂列表 14:tasklist

{
    if (0 == order)
    {
        JsonForSend(2,"",0);//获取试剂类型
    }
    else if (2 == order || 1 == order || 11 == order)
    {
        ;
    }
    else if (14 == order || 7 == order)
    {
        JsonForSend(4, "", 0); //更新归还试剂
    }
    else if (order)
    {
        JsonForSend(1, "", 0);//更新在柜试剂
    }
}


void HttpGP::EmitSignal(int status, int order)
{
    //status 判断是否对错，order 选择发送模式
    //0: 正确 —1：错误
    if (0 == status)
    {
        switch (order) {
        case 0:{

            //获取机柜信息
            break;
        }
        case 1:{
            break;
        }
        case 4:{
            msgBox->close();
            //获取带归还试剂列表信息
            break;
        }
        case 5:{
            emit sendError_To_Operate();//申请任务完成
            break;
        }
        case 6:{

            emit  sendInfo_To_executeOperate();
            break;
        }
        case 7:{

            emit sendInfo_To_sheetPage();
            break;
        }
        case 8:{

            emit sendInfo_To_executeOperate();
            break;
        }
        case 9:{

            emit sendInfo_To_executeOperate();
            break;
        }
        case 10:{

            emit sendInfo_To_sheetPage();
            break;
        }
        case 11:{

            emit sendInfo_To_Enter(0,user->user_id,user->user_role);//0:OK 1:lose
            break;
        }
        case 12:{

            emit sendInfo_To_executeOperate();
            break;
        }
        case 14:{


            break;
        }
        case 17:
        {
            printf("success---heartBeat");
        }
        default:
            break;
        }
    }
    else if (-1 == status)
    {
        switch (order) {
        case 0:{
            //获取机柜信息
            break;
        }
        case 4:{
            //获取带归还试剂列表信息
            break;
        }
        case 5:{
            emit sendError_To_Operate();//,"没有空闲位置"
            break;
        }
        case 6:{

            emit sendInfo_To_executeOperate();
            break;
        }
        case 7:{

            emit sendInfo_To_sheetPage();
            break;
        }
        case 8:{

            emit sendInfo_To_executeOperate();
            break;
        }
        case 9:{

            emit sendInfo_To_executeOperate();
            break;
        }
        case 10:{

            emit sendInfo_To_sheetPage();
            break;
        }
        case 11:{

            emit sendInfo_To_Enter(1,0,0);//request save
            break;
        }
        case 12:{

            emit sendInfo_To_executeOperate();
            break;
        }
        default:
            break;
        }
    }
    AgentiaUpdate(http_info->model_json);
}


void HttpGP::SaveDateNotSendInfo(int modelChoice, QString tableName)
{
    ;
}


















