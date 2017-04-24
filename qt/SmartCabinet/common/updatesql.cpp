#include "updatesql.h"
#include "common/globalvariable.h"
#include <QDebug>
#include <QSqlQuery>

UpdateSQL::UpdateSQL(QObject *parent) :
    QObject(parent)
{
    update_http = new NetCommunication(this);
    update_waitPage = new WaitingPage;

}

void UpdateSQL::Exec()
{
//    update_waitPage->show();
//    GetCabinetPowerList();
////    update_waitPage->close();
}

void UpdateSQL::ShowWaitPage()
{
    update_waitPage->exec();
}
//获取试剂状态列表
bool UpdateSQL::GetReagentStatusList(int *user_Id)
{
    QString json_str = "";

    json_str = update_http->PackageJson_agentiaStatusList(*user_Id, CABINETNO);
    if (update_http->PostHttp("agentiaStatusList", json_str, 2))
    {
        if (UnpackAgentiaStatusList(update_http->ServerReply))
        {
            return true;
        }
    }
    return false;
}
//获取任务状态列表
bool UpdateSQL::GetTaskList(int *user_Id, int task)
{
//    QString json_str = "";

//    json_str = update_http->PackageJson_taskList(*user_Id, CABINETNO);
//    if (update_http->PostHttp("taskList", json_str, 2))
//    {
//        if (UnpackTaskList(update_http->ServerReply, &task))
//        {
//            return true;
//        }
//    }
//    return false;
}
//获取用户权限列表
bool UpdateSQL::GetCabinetPowerList()
{
    QString json_str = "";

    json_str = update_http->PackageJson_privilegeTable(CABINETNO);
    if (update_http->PostHttp("privilegeTable", json_str, 2))
    {
        if (UnpackPrivilegeTable(update_http->ServerReply))
        {
            return true;
        }
    }
    return false;
}
//获取未归还权限列表
bool UpdateSQL::GetWaitReturnList(int *user_Id)
{
    QString json_str = "";
    json_str = update_http->PackageJson_occupiedAgentiaList(*user_Id, CABINETNO);
    if (update_http->PostHttp("occupiedAgentiaList", json_str, 2))
    {
        if (UnpackOccupiedAgentiaList(update_http->ServerReply))
        {
            return true;
        }
    }
    return false;
}
//获取版本号
bool UpdateSQL::GetVersion()
{
    QString json_str = "";

    json_str = update_http->PackageJson_version(CABINETNO);
    if (update_http->PostHttp("version", json_str, 2))
    {
        if (UnpackVersion(update_http->ServerReply))
        {
            return true;
        }
    }
    return false;
}
//获取试剂类型列表
bool UpdateSQL::GetAgentiaType()
{
    QString json_str = "";
    int type = 99;
    groupId = 1;
    json_str = update_http->PackageJson_agentiaTypeList(type, groupId);
    if (update_http->PostHttp("agentiaTypeList", json_str, 2))
    {
        if (UnpackAgetiaType(update_http->ServerReply))
            return true;
    }
    return false;
}
//获取全部试剂列表
bool UpdateSQL::GetAllAgentia(int *user_Id)
{
    QString json_str = "";
    json_str = update_http->PackageJson_agentiaList(*user_Id, CABINETNO);
    if (update_http->PostHttp("agentiaList", json_str, 2))
    {
        if (UnpackAllAgetia(update_http->ServerReply))
            return true;
    }
    return false;
}



//解析

//解析权限表
bool UpdateSQL::UnpackPrivilegeTable(QJsonDocument jd)
{
    QJsonArray s_JA;
    QJsonObject analyze_Z = jd.object();
    QJsonObject analyze_C;
    QJsonValue s_json[5] = {0};
    int s_int[4] = {0};
    bool s_success = false;

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    qDebug()<<s_success<< "  status";

    if (s_success)
    {
        QSqlQuery query;
        query.exec(QString("DELETE from T_UserPowerList"));

        s_JA = analyze_Z["privilegeTable"].toArray();

        for (int i = 0; i < s_JA.size(); i++)
        {
            analyze_C = s_JA[i].toObject();

            s_json[0] = analyze_C["roleType"].toInt();
            s_int[0] = s_json[0].toInt();

            s_json[1] = analyze_C["taskType"].toInt();
            s_int[1] = s_json[1].toInt();

            s_json[2] = analyze_C["agentiaType"].toInt();
            s_int[2] = s_json[2].toInt();

            s_json[3] = analyze_C["privilege"].toInt();
            s_int[3] = s_json[3].toInt();

            query.prepare("insert into T_UserPowerList \
                          (id,roleType,taskType,agentiaType,privilege) \
                          values \
                          (?,?,?,?,?)");
            query.addBindValue(i+1);
            query.addBindValue(s_int[0]);
            query.addBindValue(s_int[1]);
            query.addBindValue(s_int[2]);
            query.addBindValue(s_int[3]);

            query.exec();

        }
    }
    else
    {
        return false;
    }
    return true;
}

//解析试剂状态列表
bool UpdateSQL::UnpackAgentiaStatusList(QJsonDocument jd)
{
    QJsonArray s_JA;
    QJsonObject analyze_Z = jd.object();
    QJsonObject analyze_C;
    QJsonValue s_json[5] = {0};
    int s_int[4] = {0};
    bool s_success = false;

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    qDebug()<<s_success<< "  status";

    if (s_success)
    {
        QSqlQuery query;
        query.exec(QString("DELETE from T_AgentiaStatusList"));

        s_JA = analyze_Z["agentiaStatusList"].toArray();

        for (int i = 0; i < s_JA.size(); i++)
        {
            analyze_C = s_JA[i].toObject();

            s_json[0] = analyze_C["agentiaId"].toInt();
            s_int[0] = s_json[0].toInt();

            s_json[1] = analyze_C["positionId"].toInt();
            s_int[1] = s_json[1].toInt();

            s_json[2] = analyze_C["drawerId"].toInt();
            s_int[2] = s_json[2].toInt();

            s_json[3] = analyze_C["agentiaStatus"].toInt();
            s_int[3] = s_json[3].toInt();

            query.prepare("insert into T_AgentiaStatusList \
                          (id,agentiaId,positionId,drawerId,agentiaStatus) \
                          values \
                          (?,?,?,?,?)");
            query.addBindValue(i+1);
            query.addBindValue(s_int[0]);
            query.addBindValue(s_int[1]);
            query.addBindValue(s_int[2]);
            query.addBindValue(s_int[3]);

            query.exec();

        }
    }
    else
    {
        return false;
    }
    return true;
}

//解析任务列表
bool UpdateSQL::UnpackTaskList(QJsonDocument jd, int *task)
{
    QString    s_str[11]    = {0};//save string
    int        s_int[11]    = {0};//save int value
    int        s_allInfoNum = 0;//save all info number
    QJsonValue s_json[11]   = {0};//save jsonvalue
    QJsonArray s_JA;                //savef jsonArray
    QJsonObject analyze_Z = jd.object();
    QJsonObject analyze_C;
    /*****存储信息******/
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

    QSqlQuery query;

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    qDebug()<<s_success<< "  status";

    if (s_success)
    {
        int  s_taskStatus = 0;
        int  s_taskType   = 0;

        if (*task == TASK_TAKEIN)
        /*******************初始化表格***********************/

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

            /********************************将任务分别加入各自执行表格中去**************************/
            if (1 == s_taskStatus && *task == s_taskType)//审核通过
            {
                if (0 == s_taskType)//入柜
                {
                    query.prepare("insert into T_Task_PutIn (id,agentiaName,judgeAttitude,\
                                  bottleCapacity,dose,drawerSize,expireDate,roleStatus,\
                                  attribute,agentiaTypeId,chemicalFormula,specification,\
                                  factory,itemNo,positionId,drawerNo,positionNo) values (?,?,?,\
                                                                                         ?,?,?,?,?,\
                                                                                         ?,?,?,?,\
                                                                                         ?,?,?,?,?)");
                    query.addBindValue(i+1);
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
                else if (1 == s_taskType)//取用已改
                {
                    query.prepare("insert into T_AgentiaExecute (id,agentiaName,judgeAttitude,bottleCapacity,\
                                  dose,drawerNo,positionNo,expireDate,attribute,\
                                                    agentiaId,positionId) values (?,?,?,\
                                                                              ?,?,?,?,?,\
                                                                              ?,?,?)");
                    query.addBindValue(i+1);
                    query.addBindValue(s_str[3]);
                    query.addBindValue(QString("已审批"));
                    query.addBindValue(QString(""));
                    query.addBindValue(s_str[4]);
                    query.addBindValue(s_int[5]);
                    query.addBindValue(s_int[6]);
                    query.addBindValue(time);
                    query.addBindValue(3);
                    query.addBindValue(s_taskId);
                    query.addBindValue(s_taskType);

                    query.exec();
                }
                else if (2 == s_taskType)//归还已改
                {
                    query.prepare("insert into T_AgentiaReturnExecute (id,agentiaName,judgeAttitude,bottleCapacity,\
                                  dose,drawerNo,positionNo,expireDate,attribute,agentiaId,positionId)\
                            values (?,?,?,?,?,?,?,?,?,?,?)");
                    query.addBindValue(i+1);
                    query.addBindValue(s_str[3]);
                    query.addBindValue(QString("已审批"));
                    query.addBindValue(QString(""));
                    query.addBindValue(s_str[4]);
                    query.addBindValue(s_int[5]);
                    query.addBindValue(s_int[6]);
                    query.addBindValue(time);
                    query.addBindValue(3);
                    query.addBindValue(s_taskType);
                    query.addBindValue(s_taskId);

                    query.exec();
                }
                else if (3 == s_taskType)//替换
                {
                    query.prepare("insert into T_AgentiaReplace (id,agentiaName,judgeAttitude,bottleCapacity,\
                                  dose,newdose,expireDate,drawerNo,positionNo,attribute,\
                                  agentiaId,positionId) values (?,?,?,\
                                                                              ?,?,?,?,?,?,\
                                                                              ?,?,?)");
                    query.addBindValue(i+1);
                    query.addBindValue(s_str[3]);
                    query.addBindValue(QString("已审批"));
                    query.addBindValue(QString(""));
                    query.addBindValue(s_str[4]);
                    query.addBindValue(s_str[8]);
                    query.addBindValue(newTime);
                    query.addBindValue(s_int[5]);
                    query.addBindValue(s_int[6]);
                    query.addBindValue(3);
                    query.addBindValue(s_taskType);
                    query.addBindValue(s_taskId);

                    query.exec();
                }
                else if (4 == s_taskType)//报废
                {
                    query.prepare("insert into T_AgentiaScrap (id,agentiaName,judgeAttitude,bottleCapacity,\
                                  dose,drawerNo,positionNo,expireDate,attribute,\
                                  agentiaId,positionId) values (?,?,?,\
                                                                              ?,?,?,?,?,\
                                                                              ?,?,?)");
                    query.addBindValue(i+1);
                    query.addBindValue(s_str[3]);
                    query.addBindValue(QString("已审批"));
                    query.addBindValue(QString(""));
                    query.addBindValue(s_str[4]);
                    query.addBindValue(s_int[5]);
                    query.addBindValue(s_int[6]);
                    query.addBindValue(time);
                    query.addBindValue(3);
                    query.addBindValue(s_taskType);
                    query.addBindValue(s_taskId);

                    query.exec();
                }
                else if (5 == s_taskType)//点验
                {
                    query.prepare("insert into T_AgentiaCheck (id,agentiaName,judgeAttitude,bottleCapacity,\
                                  dose,newdose,expireDate,drawerNo,positionNo,attribute,\
                                  agentiaId,positionId) values (?,?,?,?,?,\
                                                                              ?,?,?,?,\
                                                                              ?,?,?)");
                    query.addBindValue(i+1);
                    query.addBindValue(s_str[3]);
                    query.addBindValue(QString("已审批"));
                    query.addBindValue(QString(""));
                    query.addBindValue(s_str[4]);
                    query.addBindValue(s_str[8]);
                    query.addBindValue(newTime);
                    query.addBindValue(s_int[5]);
                    query.addBindValue(s_int[6]);
                    query.addBindValue(3);
                    query.addBindValue(s_taskType);
                    query.addBindValue(s_taskId);

                    query.exec();
                }
            }
        }
        return true;
    }

    return false;
}

//解析未归还任务列表
bool UpdateSQL::UnpackOccupiedAgentiaList(QJsonDocument jd)
{
    QJsonArray s_JA;
    QJsonObject analyze_Z = jd.object();
    QJsonObject analyze_C;
    QJsonValue s_json[10] = {0};
    int s_int[10] = {0};
    QString    s_str[11]    = {0};//save string

    bool s_success    = false;
    QString month = "";
    QString year = "";
    QString day = "";
    QString time = "";

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    qDebug()<<s_success<< "  status";

    if (s_success)
    {
        QSqlQuery query;
        query.exec(QString("DELETE from T_AgentiaReturnSave"));

        s_JA = analyze_Z["agentiaList"].toArray();

        for (int i = 0; i < s_JA.size(); i++)
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

            query.prepare("insert into T_AgentiaReturnSave (id,agentiaName,judgeAttitude,userDisplayName,\
                          bottleCapacity,dose,drawerNo,positionNo,expireDate,attribute,\
                          agentiaId,positionId,drawerId,agentiaStatus,userId)\
                          values (?,?,?,?,?,?,?,?,\
                                  ?,?,?,?,?,?,?)");
            query.addBindValue(i+1);
            query.addBindValue(s_str[0]);
            query.addBindValue(QString("借出"));
            query.addBindValue(userName);
            query.addBindValue(s_str[1]);
            query.addBindValue(s_str[2]);
            query.addBindValue(s_int[4]);
            query.addBindValue(s_int[5]);
            query.addBindValue(time);
            query.addBindValue(s_int[8]);
            query.addBindValue(s_int[6]);
            query.addBindValue(s_int[7]);
            query.addBindValue(0);
            query.addBindValue(0);
            query.addBindValue(userId);

            query.exec();
        }
    }
    else
    {
        return false;
    }
    return true;
}

//解析版本号
bool UpdateSQL::UnpackVersion(QJsonDocument jd)
{
    QJsonObject analyze_Z = jd.object();
    QJsonValue s_json[5] = {0};
    bool s_success = false;
    QString armVersion = "";
    QString armDownLoad = "";

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    if (s_success)
    {
        QSqlQuery query;
        query.exec(QString("DELETE from T_temporary"));

        s_json[0] = analyze_Z["armVersion"].toString();
        armVersion = s_json[0].toString();

        s_json[1] = analyze_Z["armDownload"].toString();
        armDownLoad = s_json[1].toString();

        query.prepare("insert into T_temporary (versionNo,address)\
                values (?,?)");
        query.addBindValue(armVersion);
        query.addBindValue(armDownLoad);

    }
    else
        return false;

    return true;
}

bool UpdateSQL::UnpackAgetiaType(QJsonDocument jd)
{
    QJsonArray s_JA;
    QJsonObject analyze_Z = jd.object();
    QJsonObject analyze_C;
    QJsonValue s_json[10] = {0};
    int s_int[10] = {0};
    QString    s_str[10]    = {0};//save string

    int  s_allInfoNum = 0;//save all info number
    bool s_success    = false;

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    qDebug()<<s_success<< "  status";


    if (s_success)
    {
        QSqlQuery query;
        query.exec(QString("DELETE from T_AgentiaTypeList"));

        s_json[0] = analyze_Z["amount"].toInt();
        s_int[0]  = s_json[0].toInt();

        s_allInfoNum = s_int[0];

        s_JA = analyze_Z["agentiaTypeList"].toArray();

        for (int i = 0; i < s_allInfoNum; i++)
        {
            analyze_C = s_JA[i].toObject();

            s_json[1] = analyze_C["agentiaName"].toString();
            s_str[1]  = s_json[1].toString();

            s_json[2] = analyze_C["abbreviation"].toString();
            s_str[2]  = s_json[2].toString();

            s_json[3] = analyze_C["chemicalFormula"].toString();
            s_str[3]  = s_json[3].toString();

            s_json[4] = analyze_C["specification"].toString();
            s_str[4]  = s_json[4].toString();

            s_json[5] = analyze_C["factory"].toString();
            s_str[5]  = s_json[5].toString();

            s_json[6] = analyze_C["attribute"].toInt();
            s_int[6]  = s_json[6].toInt();

            s_json[7] = analyze_C["agentiaTypeId"].toInt();
            s_int[7] = s_json[7].toInt();

            s_json[8] = analyze_C["itemNo"].toString();
            s_str[8]  = s_json[8].toString();

            query.prepare("insert into T_AgentiaTypeList (id, agentiaName,\
                          abbreviation, chemicalFormula, specification,\
                          factory, attribute, agentiaTypeId, itemNo) \
                          values (?,?,?,?,?,?,?,?,?)");

            query.addBindValue(i+1);
            query.addBindValue(s_str[1]);
            query.addBindValue(s_str[2]);
            query.addBindValue(s_str[3]);
            query.addBindValue(s_str[4]);
            query.addBindValue(s_str[5]);
            query.addBindValue(s_int[6]);
            query.addBindValue(s_int[7]);
            query.addBindValue(s_str[8]);

            query.exec();
        }
    }

    else
        return false;

    return true;
}

//解析所有在柜试剂列表

bool UpdateSQL::UnpackAllAgetia(QJsonDocument jd)
{
    QJsonArray s_JA;
    QJsonObject analyze_Z = jd.object();
    QJsonObject analyze_C;
    QJsonValue s_json[15] = {0};
    int s_int[15] = {0};
    QString    s_str[15]    = {0};//save string

    bool s_success    = false;
    QString month = "";
    QString year = "";
    QString day = "";
    QString time = "";
    QString agentiaJudge = "";

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    if (s_success)
    {
        QSqlQuery query;
        query.exec(QString("DELETE from T_AgentiaSaving"));

        s_JA = analyze_Z["agentiaList"].toArray();

        for (int i = 0; i < s_JA.size(); i++)
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

            s_json[9] = analyze_C["drawerId"].toInt();
            s_int[9]  = s_json[9].toInt();

            s_json[11] = analyze_C["name"].toString();
            s_str[11]  = s_json[11].toString();

            s_json[10] = analyze_C["agentiaStatus"].toInt();
            s_int[10]  = s_json[10].toInt();
            agentiaJudge = ReturnStatus(s_int[10], s_str[11]);

            s_json[12] = analyze_C["userId"].toInt();
            s_int[12]  = s_json[12].toInt();

            if (agentiaJudge == "在位" || agentiaJudge == "借出" || agentiaJudge == "已申请" ||\
                agentiaJudge == "异常")
            {
                query.prepare("insert into T_AgentiaSaving (id,agentiaName,judgeAttitude,userDisplayName,\
                              bottleCapacity,dose,drawerNo,positionNo,expireDate,attribute,\
                              agentiaId,positionId,drawerId,agentiaStatus,userId)\
                              values (?,?,?,?,?,?,?,?,\
                                      ?,?,?,?,?,?,?)");
                query.addBindValue(i+1);
                query.addBindValue(s_str[0]);
                query.addBindValue(agentiaJudge);
                query.addBindValue(s_str[11]);
                query.addBindValue(s_str[1]);
                query.addBindValue(s_str[2]);
                query.addBindValue(s_int[4]);
                query.addBindValue(s_int[5]);
                query.addBindValue(time);
                query.addBindValue(s_int[8]);
                query.addBindValue(s_int[6]);
                query.addBindValue(s_int[7]);
                query.addBindValue(s_int[9]);
                query.addBindValue(s_int[10]);
                query.addBindValue(s_int[12]);

                query.exec();
            }

        }
    }
    else
    {
        return false;
    }
    return true;
}

QString UpdateSQL::ReturnStatus(const int &agentiaStatus, QString &name)
{
    switch (agentiaStatus) {
    case 0:
    {
        name = "";
        return "在位";
    }
    case 1:
    {
        return "借出";
    }
    case 2:
    {
        return "废弃";
    }
    case 3:
    {
        return "待入柜";
    }
    case 4:
    {
        return "已申请";
    }
    case 5:
    {
        return "异常";
    }
    default:
        break;
    }
}




