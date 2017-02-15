#ifndef OPERATEWINDOW_H
#define OPERATEWINDOW_H

#include "global_Vailable.h"
#include <QWidget>
#include <QSqlTableModel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRelationalDelegate>
#include <QSqlError>


#include <QCheckBox>
#include <QPushButton>



#include "execut_window.h"
#include "myqheaderview.h"
#include "checkboxdelegate.h"
#include "agentiainfochange.h"
#include "pbtdelegate.h"
#include "sheet_operatepage.h"
#include "httpgp.h"
#include "delaytime.h"

namespace Ui {
class OperateWindow;
}

class OperateWindow : public QWidget
{
    Q_OBJECT

public:
    explicit OperateWindow(QWidget *parent = 0);
    ~OperateWindow();

    void ModelSelect(int num);//1:入柜 2：取 3：还 4:替换 5:报废 6：点验

signals:
    void OperateWindow_To_MainUI();

    void sendInfo_To_AgentiaInfoChange(int status, int OK_Push, int sheetOrder);
    //发送信息到修改处 status 1：单个模式 0：集体模式 OK_Push 2:集体模式 1：单个模式 sheetorder 传递目标列

    void stateUp_OperatePage(int state);//更改头状态
    void sendInfo_To_agentiaInfoChangePageForEnable(int state);//使能表格状态 0 注销 1使能


private slots:
    void on_Bt_backBefore_clicked();

    void on_Bt_searchInfo_clicked();

    void on_Bt_execute_clicked();

    void on_Bt_delete_clicked();

    void on_Bt_add_clicked();

    void onState(int state);

    void receiveAddNew(int option);

    void upSheet_PutInWindow();

    void on_Bt_changeInfo_A_clicked();

    void closePage();

    void OperateError_info(void);//更新本地数据库



private:
    Ui::OperateWindow *ui;
    Execut_window *Operate_execut_Page;
    AgentiaInfoChange *AgentiaInfoChange_execute_Page;
    Sheet_OperatePage *Sheet_Operate_execute_Page;
    HttpGP *http_GP;
    DelayTime *delayTime;


    QSqlTableModel *T_model_Other_Search;
    QSqlTableModel *T_model_Other_Execut;
    QSqlQuery *query;
    MyQHeaderView *pHeader;
    ExecuteSheet *EHeader;
    ExecuteSheet_R *RHeader;
    CheckBoxDelegate *checkBoxDelegate;
    ReturnAgentiaCheckBoxDelegate* returnAgentiaChechBoxDelegate;
    BlakeDelegate *blake_Operate;
    AddNew *add_NewOperate;
    DelNew *del_NewOperate;
    ChangeNew *change_NewOperate;

    enum {ALLOWOPERATE = 3};

    void tableInit(char modelOption);//1:入柜 2：取 4：替换 5:报废（套用取模式） 6：点验（套用替换模式）
    void addInfo_Search_To_Execute(int i,int rowNum);
    void delInfo_Execute(int i);

    void updateNo(int order);//更新数据表格行列号 //1：执行框表格序列号 2：顺序重写两个表格顺序号

    void requestPositionInfo(void);//请求位置信息

    int checkSheet(int order);// check sheet position for execute
    //检查数据表格关键位置信息是否完善
    // 0：检查size  1：检查position And drawer

    struct T_Table{
        QString T_search_RelationTable;//搜索框关联表格
        QString T_execut_RelationTable;//执行框关联表格
        int     dialog_model;//区分模式
        int     executeCount;//记录状态
        int     notDrawer;//没有填试剂的数量
    }T_TABLE;

    struct T_Table *T_table;

};

#endif // OPERATEWINDOW_H
