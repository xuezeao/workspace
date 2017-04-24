#ifndef SIGNINUI_H
#define SIGNINUI_H

#include <QObject>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "dialog_controlthread.h"

class SignInUI : public QObject
{
    Q_OBJECT
public:
    explicit SignInUI(QObject *parent = 0);
 QGridLayout *pLayout;
signals:

public slots:

};
class SUI : public QGridLayout
{
    Q_OBJECT
public:

    explicit SUI()
    {
        QLabel *title = new QLabel(tr("实验室智能试剂柜"));
        QLabel *userName = new QLabel(tr("用户账号："));


//        QGridLayout *pLayout = new QGridLayout;
//        pLayout = new QGridLayout;
//        pLayout->addWidget(title,1, 0, 0, 0);
//        pLayout->addWidget(userName,2,0,0,0);
    }
};


#endif // SIGNINUI_H
