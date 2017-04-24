/*
 * task规则
 * attribute 存储 3
 * position 存储taskId
 * agentiaId 存储 taskType
 */
#ifndef SQL_SETTING_H
#define SQL_SETTING_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QDebug>


static bool createConnection()
{
       QSqlDatabase::removeDatabase("QSQLITE");//删除数据库
       QFile::remove("SmartCabinet.db");//删除数据库文件

       QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
       db.setDatabaseName("SmartCabinet.db");
       if(!db.open()) return false;
       QSqlQuery query;

       query.exec(QString("create table T_Version ([versionNo] varchar, [cabinetID] varchar)"));
       query.exec(QString("insert into T_Version values ('0.0.0','AABBCCDD')"));

       query.exec(QString("create table T_temporary ([versionNo] varchar, [address] varchar)"));


       if (!query.exec(QString("create table T_UserPowerList ([id] int primary key,\
                          [roleType] int,[taskType] int,[agentiaType] int,\
                          [privilege] int)")))
       {
            qDebug()<<"create T_UserPowerList false" ;
       }

       if (!query.exec(QString("create table T_AgentiaStatusList ([id] int primary key,\
                          [agentiaId] int,[positionId] int,[drawerId] int,\
                          [agentiaStatus] int)")))

       //用户权限列表
       {
            qDebug()<<"create T_AgentiaStatusList false" ;
       }


       if (!query.exec(QString("create table T_UserInfo ([id] int primary key,[account] varchar, [passwd] varchar,\
                              [userId] int,[role] int)")))
       {
            qDebug()<<"create T_UserInfo false" ;
       }
       //临时存储用户信息
       query.exec(QString("insert into T_UserInfo values (1,'admin', '123456', 4, 0)"));
       query.exec(QString("insert into T_UserInfo values (2,'test', '123456', 1, 0)"));
       query.exec(QString("insert into T_UserInfo values (3,'test1', '123456', 1, 0)"));
       query.exec(QString("insert into T_UserInfo values (4,'guest', '123456', 1, 0)"));

       query.exec(QString("create table T_CabinetInfo ([cabinetName] varchar,[groupId] int,[groupName] varchar,[drawerAmount] int)"));
       //长期存放柜子信息

       query.exec(QString("create table T_DrawerList ([drawerNo] int ,[drawerName] varchar,[drawerSize] varchar,[positionAmount] int,[attribute] int)"));
       //长期抽屉信息

       query.exec(QString("create table T_AgentiaInBox (id int primary key,[checkBox] varchar,[agentiaName] varchar,[bottleCapacity] varchar,[dose] varchar,[drawerNo] int,[positionNo] int,[expireDate] varchar,[attribute] int,[agentiaId] int,[positionId] int)"));
       //在柜信息(点验)

       query.exec(QString("create table T_AgentiaTypeList (id int primary key,[agentiaName] varchar,\
                          [abbreviation] varchar,[chemicalFormula] varchar,[specification] varchar,[factory] varchar,\
                          [attribute] int,[agentiaTypeId] int,[itemNo] varchar)"));
       //试剂类型
       query.exec(QString("insert into T_AgentiaTypeList values (1,'1酒精','酒精','100ml','400ml','1',1,1,'1')"));
       query.exec(QString("insert into T_AgentiaTypeList values (2,'2酒精','酒精','200ml','400ml','1',1,1,'1')"));
       query.exec(QString("insert into T_AgentiaTypeList values (3,'3酒精','酒精','300ml','400ml','1',1,1,'1')"));
       query.exec(QString("insert into T_AgentiaTypeList values (4,'4酒精','酒精','400ml','400ml','1',1,1,'1')"));
       query.exec(QString("insert into T_AgentiaTypeList values (5,'高锰酸钾','高锰酸钾','53123ml','12ml','1',2,1,'1')"));
       query.exec(QString("insert into T_AgentiaTypeList values (6,'5酒精','酒精','5230ml','3ml','1',1,1,'1')"));

       query.exec(QString("create table T_AgentiaSaving (id int primary key,[agentiaName] varchar,[judgeAttitude] varchar,[userDisplayName] varchar,\
                          [bottleCapacity] varchar,[dose] varchar,[drawerNo] int,[positionNo] int,[expireDate] varchar,\
                          [attribute] int,[agentiaId] int,[positionId] int, [drawerId] int,[agentiaStatus] int,[userId] int)"));
       //在柜试剂信息

       query.exec(QString("insert into T_AgentiaSaving values (1,'1酒精','借出','小鸭子','100ml','400ml',1,1,'2010.7.7',1,123,2,1,1,1)"));
       query.exec(QString("insert into T_AgentiaSaving values (2,'2酒精','借出','','200ml','500ml',2,2,'2010.7.7',1,123,2,2,0,0)"));
       query.exec(QString("insert into T_AgentiaSaving values (3,'3酒精','在位','','300ml','600ml',2,1,'2010.7.7',1,123,2,2,0,0)"));
       query.exec(QString("insert into T_AgentiaSaving values (4,'4酒精','在位','','400ml','700ml',1,2,'2010.7.7',1,123,2,1,0,0)"));
//       query.exec(QString("insert into T_AgentiaSaving values (5,'高锰酸钾','在位','300ml','600ml',3,3,'2010.7.7',2,123,2)"));
//       query.exec(QString("insert into T_AgentiaSaving values (6,'氯化钾','在位','400ml','700ml',3,4,'2010.7.7',2,123,2)"));
//     query.exec(QString("insert into T_AgentiaReturnSave values (1,'1酒精','未摆放','600ml','300ml',1,2,'2010.7.7',1,123,2)"));


       query.exec(QString("create table T_AgentiaReplace (id int primary key,[agentiaName] varchar,\
                          [bottleCapacity] varchar,[dose] varchar,[newdose] varchar,[expireDate] varchar,[drawerNo] int,[positionNo] int,\
                          [attribute] int,[agentiaId] int,[positionId] int,[judgeAttitude] varchar)"));
       //替换 操作数据表

       query.exec(QString("create table T_AgentiaCheck (id int primary key,[agentiaName] varchar,[judgeAttitude] varchar,\
                         [bottleCapacity] varchar,[dose] varchar,[newdose] varchar,[expireDate] varchar,[drawerNo] int,[positionNo] int,\
                         [attribute] int,[agentiaId] int,[positionId] int)"));
       //点验 操作数据表
       query.exec(QString("insert into T_AgentiaReplace values (1,'1酒精','未摆放','100ml','400ml','100ml','2010.7.7',1,1,1,123,2)"));


       query.exec(QString("create table T_AgentiaExecute (id int primary key,[agentiaName] varchar,[judgeAttitude] varchar,[userDisplayName] varchar,\
                          [bottleCapacity] varchar,[dose] varchar,[drawerNo] int,[positionNo] int,[expireDate] varchar,\
                          [attribute] int,[agentiaId] int,[positionId] int, [drawerId] int,[agentiaStatus] int,[userId] int)"));
       //取 操作数据表

       query.exec(QString("create table T_AgentiaScrap (id int primary key,[agentiaName] varchar,[judgeAttitude] varchar,\
                         [bottleCapacity] varchar,[dose] varchar,[drawerNo] int,[positionNo] int,[expireDate] varchar,\
                         [attribute] int,[agentiaId] int,[positionId] int)"));
      //报废 操作数据表


      query.exec(QString("create table T_AgentiaReturnSave (id int primary key,[agentiaName] varchar,[judgeAttitude] varchar,[userDisplayName] varchar,\
                         [bottleCapacity] varchar,[dose] varchar,[drawerNo] int,[positionNo] int,[expireDate] varchar,\
                         [attribute] int,[agentiaId] int,[positionId] int, [drawerId] int,[agentiaStatus] int,[userId] int)"));


                 query.exec(QString("insert into T_AgentiaReturnSave values (1,'1酒精','完成','小鸭子','100ml','400ml',1,1,'2010.7.7',1,123,2,1,1,1)"));
                 query.exec(QString("insert into T_AgentiaReturnSave values (2,'2酒精','完成','','200ml','500ml',2,2,'2010.7.7',1,123,2,2,0,0)"));

      query.exec(QString("create table T_AgentiaReturnExecute (id int primary key,[agentiaName] varchar,[judgeAttitude] varchar,[userDisplayName] varchar,\
                        [bottleCapacity] varchar,[dose] varchar,[drawerNo] int,[positionNo] int,[expireDate] varchar,\
                        [attribute] int,[agentiaId] int,[positionId] int, [drawerId] int,[agentiaStatus] int,[userId] int)"));

       //还 操作数据表 primary key

       query.exec(QString("create table T_AgentiaSearchInfo (id int primary key,[agentiaName] varchar,\
                          [bottleCapacity] varchar,[dose] varchar,[drawerNo] int,[positionNo] int,[expireDate] varchar,\
                          [currentUserName] varchar,[agentiaStatus] varchar,[attribute] varchar,[applicant] varchar,\
                          [applyStatus] int,[agentiaId] int,[positionId] int)"));
       //搜索信息


      query.exec(QString("create table T_AgentiaTypeList (id int primary key,[agentiaName] varchar,\
                         [abbreviation] varchar,[chemicalFormula] varchar,[specification] varchar,[factory] varchar,\
                         [attribute] int,[agentiaTypeId] int,[itemNo] varchar)"));
      //试剂类型
      query.exec(QString("insert into T_AgentiaTypeList values (1,'1酒精','酒精','100ml','400ml','1',1,1,'1')"));
      query.exec(QString("insert into T_AgentiaTypeList values (2,'2酒精','酒精','200ml','400ml','1',1,1,'1')"));
      query.exec(QString("insert into T_AgentiaTypeList values (3,'3酒精','酒精','300ml','400ml','1',1,1,'1')"));
      query.exec(QString("insert into T_AgentiaTypeList values (4,'4酒精','酒精','400ml','400ml','1',1,1,'1')"));
      query.exec(QString("insert into T_AgentiaTypeList values (5,'高锰酸钾','高锰酸钾','53123ml','12ml','1',2,1,'1')"));
      query.exec(QString("insert into T_AgentiaTypeList values (6,'5酒精','酒精','5230ml','3ml','1',1,1,'1')"));

       query.exec(QString("create table T_PutInExec (id int primary key, [agentiaName] varchar,\
                          [judgeAttitude] varchar,[drawerNo] int ,[positionNo] int, [bottleCapacity] varchar, [dose] varchar,\
                          [expireDate] varchar,[agentiaTypeId] int,[positionId] int)"));


       query.exec(QString("create table T_WaitPostInfo (id int primary key,[userId] int,[agentiaId] int, \
                          [positionId] int ,[dose] varchar,[expireDate] varchar,[bottleCapacity] varchar, [judgeAttitude] varchar)"));
       //存储未成功上传信息 操作数据表      //2：入柜 3:取 4：还 4：替换 5：报废 judgeAttitude

       query.exec(QString("create table Task_sheet (id int primary key,[taskId] int,[taskType] int,\
                         [taskStatus] int,[expiryDate] varchar,[agentiaName] varchar, [dose] varchar,[positionNo] varchar,[drawerNo] varchar,\
                         [cabinetNo] varchar,[newAgentia_dose] varchar,[newAgentia_expiryDate] varchar)"));

     //任务存储

       return true;

}
#endif // SQL_SETTING_H
