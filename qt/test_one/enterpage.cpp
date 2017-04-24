#include "enterpage.h"
#include "ui_enterpage.h"
#include <QDesktopWidget>
//#include "inputnew/frminputnew.h"

EnterPage::EnterPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnterPage)
{
    ui->setupUi(this);
    mainUI_Page = new MainUI;
    http_Page = new HttpGP;

//    ui->graphicsView->autoFillBackground();
//        post_Info="0";
//        postHttp(1,post_Info);//获取所有信息
//        ModelOperate=1;

//    this->showFullScreen();//主屏幕最大化

//    int width = QApplication::desktop()->width();
//    int height = QApplication::desktop()->height();
//    setGeometry(0, 0, width, height);

//    setWindowFlags(/*Qt::Tool |*/ Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

//    ui->widget->setObjectName("widget");
//    ui->widget->setStyleSheet("#widget{"
//                              "font-size:16px;"
//                              "border-image:url(:/image/1.jpg);"
//                              "}");

//    showMaximized();
//    setWindowFlags(Qt::FramelessWindowHint |Qt::Window);
    this->setGeometry(qApp->desktop()->availableGeometry());//最大化
    http_Page->GetHttp();//获取机柜消息

    connect(http_Page,SIGNAL(sendInfo_To_Enter(int,int,int)),this,SLOT(receiverInfo_from_Http(int,int,int)));
    connect(mainUI_Page,SIGNAL(leaveOperate_To_enter()),this,SLOT(closePage()));
//    frmInputNew::Instance()->init("control", "black", 12, 10, 700, 230, 20, 20, 6, 45);


}

EnterPage::~EnterPage()
{
    delete ui;
}

void EnterPage::on_pushButton_sigin_clicked()
{
    handleInfo();

}

void EnterPage::handleInfo()//处理输入框的信息
{
//    ui->pushButton_sigin->setEnabled(false);

    QString username;
    QString password;

    username = ui->lineEdit_acount->text();
    password = ui->lineEdit_password->text();

    http_Page->JsonForSend(11,username,0);
    http_Page->JsonForSend(11,password,1);
}

void EnterPage::receiverInfo_from_Http(int status, int userId, int role)
//接受服务器下发信息 0:正确 1：错误
{
    if(status == 0)//正确
    {
        qDebug()<<userId;

        QString username;

        username =ui->lineEdit_acount->text();

        mainUI_Page->show();
        mainUI_Page->initShow(role,username);

        this->close();
    }
    else if(status == 1) //错误
    {
        QMessageBox::warning(this,tr("Waring"),tr("user name or password error!"),QMessageBox::Yes);
    }

    ui->lineEdit_acount->clear();
    ui->lineEdit_password->clear();
    ui->pushButton_sigin->setEnabled(true);
}


void EnterPage::closePage()
{
    this->show();
}

void EnterPage::updateCabinetInfo()
{
    ;
}

