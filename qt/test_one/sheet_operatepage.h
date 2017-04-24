#ifndef SHEET_OPERATEPAGE_H
#define SHEET_OPERATEPAGE_H

#include <QDialog>
#include <QSqlTableModel>
#include "httpgp.h"
#include "uart4stm.h"

namespace Ui {
class Sheet_OperatePage;
}

class Sheet_OperatePage : public QDialog
{
    Q_OBJECT

public:
    explicit Sheet_OperatePage(QDialog *parent = 0);
    ~Sheet_OperatePage();

    void modelOption(int order );   //选择模式对象 1:取 2:报废

signals:

    void closePrevious();

private slots:

    void on_pBt_return_clicked();

    void on_pBt_OK_clicked();

    void receivers_From_http();

    void on_pBt_ignore_clicked();

private:
    Ui::Sheet_OperatePage *ui;

    QSqlTableModel *T_model_execute;
    HttpGP *http_GAndP_sheetOperate;

    void sheetTableInit(int num);//选择模式
    //更改试剂信息 name 表格名 i 对应的行数 just 写入的状态值
    void changeInfo(int just,int i);//更改试剂信息 1: 上传成功 2：上传失败
    void delSheetInfo(int status,QString name ,int i);//delete or empty sheet info
    void savePostFalse(QString name, int i);//存储上传失败信息
    void getDrawer();//得出抽屉号和数量
    int  getRead(char order);//返回值1:得 抽屉号和数量 2:得 试剂位置和总数
                            //3:得 post信息
    void getWriter(int i,int model);//change sheet data
                                    //model 0:已摆放 1：未摆放 2：误取 3：误放
    void closeOperate();//close page operate
    void sendHttp();
    void specialWrite(int order ,int error, int i);//写特殊情况的状态//0：未操作 1：错误 2：移动
    void sendOrder_to_STM(int status);//status 0:成功 2：查询
    void pBtStatus(int status);//update whole window 直接调用就可以查询
                               //正常发送 1:调用下发指令 2：调用查询指令
    void updateLabel(int i);// show label i/j
    void showNeedData(int i,char order);//show operating agentia
    //0:show current drawer agentia  // 1:show all drawer agentia

    int SCI_send(int order);//指令下发 //0:下发 1：查询 2：完成 error//0: 错误 1：未完成 2：正确
    void InfoError(int num, QString error);//输出任务状态并执行未成功数据保存
    //任务完成情况 0-error  1-操作OK 2-跳过 3 显示OK

    struct T_table{
        int rowid;
        int error;
        int modelOperate;
        QString T_tablesheet;
    };

    struct T_table * T_tableexecute;

    int sheet_drawer[7];//存储抽屉号和总数
    int sheet_position[17];//存储当前试剂位置和总数
    int sheet_model;//记抽屉数
    int sheet_status;//0:下发指令 1：空缺 2：查询指令 3:关闭

};

#endif // SHEET_OPERATEPAGE_H
