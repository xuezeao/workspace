#ifndef HTTPGP_H
#define HTTPGP_H


#include <QtDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QInputMethod>

/****************http******************/
#include <QByteArray>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

/**************json使用******************/
#include <QSqlTableModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

/************数据库***************/
#include <QSqlQuery>

/****************线程操作*******************/


class HttpGP : public QObject
{
    Q_OBJECT
public:
    explicit HttpGP(QObject *parent = 0);
    QNetworkAccessManager *accessManager;

//    void GetHttp(int order);//1:获取机柜信息  2：获取心跳包 3：服务器时间

    void AgentiaUpdate(int order);//更新常用数据 0：机柜信息 1：在位试剂 2：试剂类型 4：待归还试剂列表 14:tasklist

    void JsonForSend(int model_json, QString T_tableName, int allTaskCount);
    //赋值
    //model_json 选择打包json格式，T_tableName用于选择对应的表格，allTaskCount总任务行数

signals:

    void sendError_To_Operate();//发送上传状态 0：ok 1:lose
    void sendError_To_Execut(int error_status,QString errorInfo);//发送任务完成情况 0-error 1-OK
    void sendInfo_To_sheetPage();//发送上传状态 0:OK 1:lose
    void sendFalse();
    void sendInfo_To_executeOperate();//
    void sendInfo_To_Enter(int status,int userId,int userRole);//0 成功 1 失败

public slots:
    void finished(QNetworkReply *reply);
    void GetHttp(int order);//1:获取机柜信息  2：获取心跳包 3：服务器时间


private:
    QSqlQuery query;

    QMessageBox *msgBox;



    enum {ALLOWOPERATE = 3};

    struct User
    {
        int user_id;
        int user_role;//0-创建者,1-管理员,2-普通组员
        int user_groupId;
        int positionId_allocPosition;//存储系统分配positionId

        QString userName;
    };
    struct User *user;

    struct Http_info
    {
        int http_modelChoice;
        int model_json;
        int tableNo;
        int taskRole;
        //5：分配位置 6：入柜完成上报 7：取完成上报 8：还上报 9：替换
        //10：报废 11：登入 12:点验 13：报警 14:任务列表 15:网络任务完成上报
        int allTaskCount;

        QString tableName;
    };
    struct Http_info *http_info;

    int taskHandleCount;

    void PostHttp(int postId_NO, QString postStr);//postName_NO用于选择接口名对应的序号，postStr用于装载需要发送内容
    //0:获取机柜信息 1：获取在位试剂 2：获取试剂类型 4：获取待归还试剂 5:请求空闲位置
    //6:入柜完成 7：取完成  8：还完成  9：替换完成 10：报废完成 11：登入 12：点验 13：报警信息 14：任务列表

    void PackageJson(int model_json, QString T_tableName, int T_tableNo);
    //封装JSON信息
    //0:获取机柜信息 1：获取在位试剂 2：获取试剂类型 4：获取待归还试剂  5:请求空闲位置
    //6:入柜完成 7：取完成  8：还完成  9：替换完成 10：报废完成 11：登入 12：点验 13：报警信息 14：任务列表 15：网络任务完成上报
    //17:心跳包 18:服务器时间
    void PackageJsonForTask(int taskId);
    //封装JSON申请任务完成
    //15:网络任务完成上报

    int UnpackageJson(QJsonDocument str, int t);
    //解析JSON信息并存入数据库
    // 0:获取机柜信息 1：获取在位试剂 2：获取试剂类型 4：获取待归还试剂
    //5：分配位置 6：入柜完成上报 7：取完成上报 8：还上报 9：替换 10：报废 11：登入 12:点验 13：报警14:任务列表  15：网络任务完成上报


    void JuageOperatorStatus(int order);
    //-2:网络错误  -1：任务失败 0：任务完成
    //1：获取在位试剂 2：获取试剂类型 4：获取待归还试剂
    //5：分配位置 6：入柜完成上报 7：取完成上报 8：还上报  9：替换 10：报废 11：登入 12:点验
    //13：报警 14：任务列表 15：网络任务完成上报 16：获取柜子信息

    void HttpInit(void);
    //初始化

    void EmitSignal(int status, int order);
    //status 判断是否对错，order 选择发送模式
    //0: 正确 —1：错误

    void SaveDateNotSendInfo(int modelChoice, QString tableName);
    //存储上传失败信息到数据库中暂存
    //modelChoice 选择模式 tablename 表格名



};

#endif // HTTPGP_H
