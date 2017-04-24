#ifndef ENTERPAGE_H
#define ENTERPAGE_H

#include <QWidget>
#include <QPixmap>
#include <QMessageBox>
#include "mainui.h"
#include "httpgp.h"

namespace Ui {
class EnterPage;
}

class EnterPage : public QWidget
{
    Q_OBJECT

public:
    explicit EnterPage(QWidget *parent = 0);
    ~EnterPage();

private slots:
    void on_pushButton_sigin_clicked();

    void receiverInfo_from_Http(int status,int userId,int role);//接受服务器下发信息 0:正确 1：错误

    void closePage();

private:
    Ui::EnterPage *ui;
    MainUI *mainUI_Page;
    HttpGP *http_Page;

    void handleInfo();//处理输入框的信息
    void updateCabinetInfo();//更新试剂信息
};

#endif // ENTERPAGE_H
