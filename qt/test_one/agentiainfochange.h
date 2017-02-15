#ifndef AGENTIAINFOCHANGE_H
#define AGENTIAINFOCHANGE_H

#include <QDialog>

namespace Ui {
class AgentiaInfoChange;
}

class AgentiaInfoChange : public QDialog
{
    Q_OBJECT

public:
    explicit AgentiaInfoChange(QDialog *parent = 0);
    ~AgentiaInfoChange();
signals:
    void upSheet_PutIn();

private slots:
    void on_pBt_returnPrevious_clicked();

    void on_pBt_nextOperate_clicked();

    void on_pBt_previousOperate_clicked();

    void sendUP_Info(int status, int OK_Push, int sheetOrder);
    //收信息到修改处 status 1：单个模式 0：集体模式 OK_Push 2:集体模式 1：单个模式 sheetorder 传递目标列
    void receiverUpEnable(int status);//使能编辑框功能//0:注销 换 1：恢复 入柜

private:
    Ui::AgentiaInfoChange *ui;
    void changeShowInfo(int i);//显示信号
    void addInfo_To_sheet(int i);//把人为更改信息加入到数据库中去
    void closeWindow_operate();//执行关窗口程序

    int AInfo_sheetOrder;
    int AInfo_OK_Push;//1 完成 下一步；2 完成 上一步；0 未完成
    int modelChoice;
    QString changeTabel;//待修改的数据表格

    void dateUp(QString dateTime);//显示表格时钟
    void checkInfo(int status);//检查所需信息0：提交前检查 1：提交申请前单条 2：执行前位置ID检查

//    void next_Info_Agentia();
//    void previous_Info_Agentia();


};

#endif // AGENTIAINFOCHANGE_H
