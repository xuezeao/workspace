#include "dg_loginpage.h"
#include "ui_dg_loginpage.h"
#include "common/globalvariable.h"
#include "network/netcommunication.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QSqlQuery>

Dg_LoginPage::Dg_LoginPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dg_LoginPage)
{
    ui->setupUi(this);
    is_open = false;
    netWork = new NetCommunication(this);
#ifndef TEST
    this->showFullScreen();
#endif
//      showMaximized();
//      setWindowFlags(Qt::FramelessWindowHint);
}

Dg_LoginPage::~Dg_LoginPage()
{
    delete ui;
}

void Dg_LoginPage::on_pB_login_clicked()
{
    QString userAccount = "";
    QString userPassWD = "";
    int user_Id = 0;
    int user_role = 0;
    QString json_str = "";
    int netStatus = 0;

    userAccount = ui->lE_account->text();
    userPassWD  = ui->lE_passwd->text();

    json_str = netWork->PackageJson_login(userAccount, userPassWD);//json 打包
    netStatus = netWork->PostHttp("login", json_str, 2);//post 登陆 设置超时时间2s

    if ((NETWORK_ONLINE == netStatus) && UnpackageJson_Login(netWork->ServerReply, user_Id, user_role))
    {
        OpenMainOptPage(userAccount, user_Id, user_role);
    }
    else if ((NETWORK_OFFLINE == netStatus) && QuerySQL(userAccount, userPassWD, user_Id, user_role))
    {
        OpenMainOptPage(userAccount, user_Id, user_role);
    }
    else
    {
        QMessageBox::warning(NULL, "ERROR", QString("%1").arg(QString("账号或密码错误")));
    }
}


bool Dg_LoginPage::UnpackageJson_Login(QJsonDocument str, int &user_Id, int &user_role)
{
#ifdef TEST
    qDebug()<<str;
#endif

    QJsonValue s_json[5] = {0};
    bool s_success = false;

    QJsonObject analyze_Z = str.object();

    s_json[0] = analyze_Z["success"].toBool();
    s_success = s_json[0].toBool();

    if (s_success)
    {
        s_json[0] = analyze_Z["userId"].toInt();
        user_Id = s_json[0].toInt();

        s_json[1] = analyze_Z["role"].toInt();
        user_role = s_json[1].toInt();
    }

    str = QJsonDocument::fromJson(QString(" ").toUtf8());

    return s_success;

}

void Dg_LoginPage::OpenMainOptPage(QString usrName, int &user_Id, int &user_role)
{

    userName = "";
    userId = 0;
    userRole = 0;

    userName = usrName;
    userId   = user_Id;
    userRole = user_role;

    functionOptPage = new Dg_FunctionOptPage(this);
    functionOptPage->show();
    this->hide();
    is_open = true;

    connect(functionOptPage, SIGNAL(destroyed()), this, SLOT(ShowLoginPage()));
}

void Dg_LoginPage::Close_ActivePage()
{
   if (is_open)
   {
       functionOptPage->deleteLater();
   }
   else
   {
       qDebug() << "not open functionOPT";
   }
}

void Dg_LoginPage::ShowLoginPage()
{
    this->show();
    is_open = false;

    ui->lE_passwd->clear();
    ui->lE_account->clear();
}

bool Dg_LoginPage::QuerySQL(const QString &account, const QString &passwd, int &user_Id, int &user_role)
{
    QSqlQuery query(QString("SELECT * FROM T_UserInfo WHERE account = '%1'").arg(account));
    query.next();

    QString userAccount = "";
    QString userPasswd = "";

    userAccount = query.value(1).toString();
    userPasswd = query.value(2).toString();


#ifdef TEST
    qDebug() << userAccount << "  " << userPasswd;
#endif

    if ((account == userAccount) && (account != ""))
    {
        if ((passwd == userPasswd) && (passwd != " "))
        {

            user_Id = query.value(3).toInt();
            user_role = query.value(4).toInt();

            return true;
        }
    }

    return false;
}

void Dg_LoginPage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        resetSignal = RESETTIMER;
    }
}


void Dg_LoginPage::on_pB_quit_clicked()
{
    this->deleteLater();
}
