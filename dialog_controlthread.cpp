#include "dialog_controlthread.h"
#include "ui_dialog_controlthread.h"
#include "wthread.h"

Dialog_ControlThread::Dialog_ControlThread(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ControlThread)
{
    ui->setupUi(this);

    QPushButton *A = new QPushButton(tr("A测试"));
    QPushButton *B = new QPushButton(tr("B测试"));
    QPushButton *C = new QPushButton(tr("C测试"));

    QVBoxLayout *vBL = new QVBoxLayout;
    vBL->addWidget(A);
    vBL->addWidget(B);

    vBL->addWidget(C);




    setLayout(vBL);

    connect(A, SIGNAL(clicked()), this, SLOT(A_pushButton()));
    connect(B, SIGNAL(clicked()), this, SLOT(B_pushButton()));
    connect(C, SIGNAL(clicked()), this, SLOT(C_pushButton()));

    timer = new QTimer;
//    connect(timer, SIGNAL(timeout()), this, SLOT(A_pushButton()));
//    timer->start(5000);

    thread_http = new QThread;
    thread_query = new QThread;
    httpA = new HttpGP;
    seriportQuery = new SeriportQuery;

    httpA->moveToThread(thread_http);
    seriportQuery->moveToThread(thread_query);

//    thread_http->start();
    thread_query->start();
    connect(this, SIGNAL(startLoop()), seriportQuery, SLOT(requestSTM()));
    connect(this, SIGNAL(http(int)), httpA, SLOT(GetHttp(int)));

    WThread hread;
    hread.start();

}

Dialog_ControlThread::~Dialog_ControlThread()
{
    delete ui;
}

void Dialog_ControlThread::A_pushButton()
{
    emit http(2);
}
void Dialog_ControlThread::B_pushButton()
{
    emit http(3);
}
void Dialog_ControlThread::C_pushButton()
{
    emit startLoop();
}
