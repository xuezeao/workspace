#ifndef DIALOG_CONTROLTHREAD_H
#define DIALOG_CONTROLTHREAD_H

#include <QDialog>
#include <QThread>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include "httpgp.h"
#include "httptest.h"
#include "seriportquery.h"
#include "signinui.h"
namespace Ui {
class Dialog_ControlThread;
}

class Dialog_ControlThread : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_ControlThread(QWidget *parent = 0);
    ~Dialog_ControlThread();

public slots:
    void A_pushButton(void);
    void B_pushButton(void);
    void C_pushButton(void);
signals:
    void startLoop();
    void http(int order);
private:
    Ui::Dialog_ControlThread *ui;
    QThread *thread_http;
    QThread *thread_query;
    QThread *thread_httpTest;
    QTimer *timer;
    HttpGP *httpA;
    HttpTest *httpB;
    SeriportQuery *seriportQuery;


};

#endif // DIALOG_CONTROLTHREAD_H
