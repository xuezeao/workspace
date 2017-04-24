#ifndef DG_EXECTAKEIN_H
#define DG_EXECTAKEIN_H

#include <QDialog>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QThread>
#include "common/serialportcontrol.h"
#include "network/netcommunication.h"


namespace Ui {
class Dg_ExecTakeIn;
}

class Dg_ExecTakeIn : public QDialog
{
    Q_OBJECT

public:
    explicit Dg_ExecTakeIn(QWidget *parent = 0);
    ~Dg_ExecTakeIn();
    enum{CabinetError, OperateError, PositionError, DrawerError};
    enum allControl{serialPortOpenFail, serialPortOpenSuccess,jumpCurrentAct, haveErrorHandle,\
                    haveNotExecTask, havetaskOver, nextTask, actSendFail};

    enum Lock_S{taskActing, clickReturn};


signals:
    void TaskAct_Send(int DID, int send_ActNum);
    void OpenSerialPort();
    void GetLockStatus(int DID);
    void Unlocksignal(int DID);
    void StopTask(int DID);
    void QuitExecPage();


private slots:
    void on_pB_sure_clicked();

    void on_CB_return_clicked();

    void on_pB_jump_clicked();

    void ErrorMessage(int order);
    void HandleReply(int order);
    void LockStatusSlots(int order);
    void ClosePage();

private:
    Ui::Dg_ExecTakeIn *ui;

    QSqlTableModel *t_exec;
    QSqlQuery *query;
    QThread *threadSerialPort;
    SerialPortControl *serialPort;
    QTimer *timer;
    NetCommunication *netCommunication;

    int s_drawerColumn;
    int s_positionColumn;
    int s_judgeStatusColumn;
    int s_orderColumn;
    int s_doseColumn;
    int s_agetiaTypeIdColumn;
    int s_positionIdColumn;
    QString s_execTable;

    QMap <int, int> save_drawer;
    QMap <int, int> save_position;
    QMap <int, int> save_number;
    QMap <int, int> save_previousErrorPosition;
    QList<int> save_warningInfo;

    struct Save_AgentiaInfo{
        int getC_agentiaId;
        int getC_position;
        QString getC_dose;
        QString getC_bottleCapacity;
        QString getC_expireDate;
        QString getC_judgeAttitude;
    };
    Save_AgentiaInfo *agetiaInfo;


    bool isOpenSerialPort;
    int count;
    int lockStatus;
    int preDrawerNo;

    //设置标题
    void SetTitle(QString name);
    //总处理
    void ProcessHandle();
    //显示界面，排序
    void ShowPage();
    //获取位置号
    void GetDrawerAndPositionNum();
    //加载下一条任务信息
    void ShowCurrentNeedExecuteTaskInfo(int i);
    //创建子线程
    void CreateAndStartThread();
    //上报任务信息
    void UploadTask2Server();
    void UploadWarning(QString json_str, QString warningContent, int model);
    void UploadDisableDrawerAndPosition(int model);
    //解析json
    bool UnpackageJson_Task(QJsonDocument str);
    //创建串口
    void CreateSerialPort();
    //修改单元格内容
    void ChangeCellContent(int row, int column, QString content);
    void ChangeStatus(QString content);
    //设置提示框内容
    void TextMessageShowContent(QString colorType, QString content);
    //初始化结构体
    void InitializeStruct();
    //存储数据库
    void WriteSQL_NetworkError();
    //读取异常信息
    void GetError();
    //总控台
    void AllControl(int order);
    //检测错误是否存在
    void isSaveError(int goal);
    //下一条任务
    void NextTask();
    //等待当前锁关闭
    void RequestLockStatus_Current(int i);
    //任务下发
    void TaskSend();
    //等待
    void waitTime(int time);

};

#endif // DG_EXECTAKEIN_H
