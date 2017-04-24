#ifndef DG_LOGINPAGE_H
#define DG_LOGINPAGE_H

#include <QDialog>
#include "network/netcommunication.h"
#include "ui/common/dg_functionoptpage.h"
#include <QJsonDocument>

namespace Ui {
class Dg_LoginPage;
}

class Dg_LoginPage : public QDialog
{
    Q_OBJECT

public:
    explicit Dg_LoginPage(QWidget *parent = 0);
    ~Dg_LoginPage();



public slots:
    void on_pB_login_clicked();

    void ShowLoginPage();
    void Close_ActivePage();

private slots:
    void on_pB_quit_clicked();

private:
    Ui::Dg_LoginPage *ui;
    NetCommunication *netWork;
    Dg_FunctionOptPage *functionOptPage;

    bool is_open;

    bool UnpackageJson_Login(QJsonDocument str, int &user_Id, int &user_role);
    void OpenMainOptPage(QString usrName, int &user_id, int &user_role);
    bool QuerySQL(const QString &account, const QString &passwd, int &user_Id, int &user_role);
    void mousePressEvent(QMouseEvent *event);

};

#endif // DG_LOGINPAGE_H
