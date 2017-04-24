#ifndef EXECUT_WINDOW_H
#define EXECUT_WINDOW_H

#include <QDialog>
#include "httpgp.h"
#include "showallinfo.h"
#include "uart4stm.h"


namespace Ui {
class Execut_window;
}

class Execut_window : public QDialog
{
    Q_OBJECT

public:
    explicit Execut_window(QDialog *parent = 0);
    ~Execut_window();

     void executeInit(int num);
     //1:入柜 2：归还 4:替换 6：点验

signals:
     void upSheet_From_Execute();

private slots:
    void on_pBt_cancal_clicked();

    void on_pBt_next_clicked();

    void NetworkError();//网络连接错误

    void updateReturn();//0 成功 1 失败 2 未放置

    void on_pBt_ignore_clicked();
      void closePage();//关闭页面操作

private:
    Ui::Execut_window *ui;
    HttpGP *Execut_http_GAndP;
    ShowAllInfo *Show_Info;

    void initVariable(void);//初始化变量


    void pBt_operate(int order);//按钮操作 //0：下一步 1：查询
    void operateNext(void);//执行下一步

    void http_PG_AgentiaInfo(int order);//上传/获取 药剂信息   //1:入柜 2：归还 4:替换 6：点验
    void changeAgentiaStatus(int just,int order);//更改药剂状态 order 内部区分 0:无操作 1：正常 2：跳过
    //更新试剂显示状态 just 1：上传成功 2：上传失败 3：未操作 4：正确操作 5：错误操作 6:报废操作
    void getAgentiaPositionInfo(int i);//获取要求位置 //1：入柜 尺寸 2：还 位置
    void updateShowInfo(QString A_name,QString Volume,QString Position);//更改显示信息

    int  SCI_send(int order);//0:下发包含开锁 1：查询 2：完成 3:查锁 int Error;//0: 错误 1：未完成 2：正确
    void searchPositionInfo(QString name, int i);//检索药剂位置
    void getAllCount(void);//获取总的行数
    void moveAgentia();//误操作信息记录
    void executeInfoError(int num,QString error);//任务完成情况 0-error  1-OK 2-跳过 输出任务状态并执行未成功数据保存
    void waitTaskInfo(int tim);//延时ms
    void checkLockStatus(void);//检测锁的状态

    struct Execute_Variable{
        int execute_model;//1：入柜 2：还
        int acountRow;//表格总row -1
        int currentAcount;//记入执行次数
        int drawerAcount;//记入执行次数
        int judgeStatus;//用于判断状态 0: 第一次 1：第二次
        char statusOption; // 0 误取，1 误放，2 未操作，3 正确操作

        int positionNO;
        int drawerNo;
        int positionId;
        QString T_executeTable;
        QString newDose;

        int orderPosition;//计rowid
        char pBt_status;//1：可发送查询 2：等待执行

        int test;

    };

    struct Execute_Variable *execute_V;

    int saveDrawer[33];
    int saveRowid[33];

};

#endif // EXECUT_WINDOW_H
